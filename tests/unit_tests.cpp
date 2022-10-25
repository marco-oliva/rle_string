//
// Created by marco on 10/25/22.
//

#define CATCH_CONFIG_RUNNER
#include <catch2/catch_all.hpp>

#include <rle/rle_string.hpp>

//------------------------------------------------------------------------------

struct listener : Catch::EventListenerBase
{
    using EventListenerBase::EventListenerBase;

    virtual void testCaseStarting(Catch::TestCaseInfo const& testInfo) override
    {
        std::cout << testInfo.tagsAsString() << " " << testInfo.name << std::endl;
    }
};
CATCH_REGISTER_LISTENER(listener)

//------------------------------------------------------------------------------

std::string testfiles_dir = "../tests/files";

//------------------------------------------------------------------------------

rle::string_type random_string(rle::size_type length, rle::string_type seed = {})
{
    std::random_device rd;
    std::mt19937 generator(rd());

    if (seed.empty())
    {
        std::string str;
        str.append(length / 5, 'A'); str.append(length / 5, 'C'); str.append(length / 5, 'G');
        str.append(length / 5, 'T'); str.append(length / 5, 'N');
        std::shuffle(str.begin(), str.end(), generator);
        str.push_back('$');
        rle::string_type out; for (auto& c : str) { out.push_back(c); }
        return out;
    }
    else
    {
        std::uniform_real_distribution<> dist(0, 1);
        std::uniform_real_distribution<> dist_char(0, 5);
        std::string dict = "ACGTN";

        double error_rate = 0.15;
        rle::string_type str;
        for (auto& c : seed)
        {
            double rand = dist(generator);
            if (rand < error_rate) { str.push_back(dict[std::floor(dist_char(generator))]); }
            else { str.push_back(c); }
        }
        str.push_back('$');
        return str;
    }

}

//------------------------------------------------------------------------------

TEST_CASE("Bitvector", "BitvectorEmptyConstructor")
{
    rle::bitvector bitvector;

    REQUIRE(bitvector.size() == 0);
}

TEST_CASE("Bitvector", "Rank")
{
    std::vector<bool> set = {0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1};

    rle::bitvector bitvector(set);

    REQUIRE(bitvector.size() == 12);

    REQUIRE(0 == bitvector.rank(1));
    REQUIRE(0 == bitvector.rank(2));
    REQUIRE(1 == bitvector.rank(3));
    REQUIRE(1 == bitvector.rank(5));
    REQUIRE(3 == bitvector.rank(7));
    REQUIRE(5 == bitvector.rank(11));
    REQUIRE(6 == bitvector.rank(12));
}

TEST_CASE("Bitvector", "Select")
{
    std::vector<bool> set = {0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1};

    rle::bitvector bitvector(set);

    REQUIRE(bitvector.size() == 12);

    REQUIRE(2 == bitvector.select(0));
    REQUIRE(7 == bitvector.select(3));
    REQUIRE(11 == bitvector.select(5));
}

TEST_CASE("RLEString", "FromEncoder")
{
    std::string path = testfiles_dir + "/test.rle";
    rle::string_type string = {'A','A','A','G','C','A','A','T','T','T','T','T','T','A','A','G','G','A','A','A'};
    rle::RLEString rlet_string(string);


    rle::RLEString::RLEncoder encoder(path);
    for (auto e : string) { encoder(e); }
    encoder.close();

    rle::RLEString rle_string;
    rle_string.load(path);

    REQUIRE(string.size() == rle_string.size());
    for (std::size_t i = 0; i < string.size(); i++)
    {
        REQUIRE(string[i] == rle_string[i]);
    }
}

TEST_CASE("RLESEncoder", "RandomSequence")
{
    for (rle::size_type r = 0; r < 10; r++)
    {
        std::string path = testfiles_dir + "/rnd_seq.seq";
        rle::string_type rnds = random_string(10000);

        rle::RLEString::RLEncoder encoder(path);

        for (auto& e : rnds) { encoder(e); }
        encoder.close();

        rle::RLEString rle_string;
        rle_string.load(path);

        for (rle::size_type i = 0; i < rnds.size(); i++)
        {
            REQUIRE(rnds[i] == rle_string[i]);
        }
    }
}

TEST_CASE("RLEString", "FromConcatRunInterruption")
{
    std::string path = testfiles_dir + "/test_cnct.seq";
    rle::RLEString::RLEncoderMerger encoders(path, 2);

    rle::string_type string_1 = {'A','A','A','G','C','A','A','T','T','T','T','T','T','A','A','G','G','A','A','A'};
    rle::string_type string_2 = {'G','A','A','G','C','A','A','T','T','T','T','T','T','A','A','G','G','A','A','A'};

    rle::string_type string;
    string.reserve(string_1.size() + string_2.size());
    string.insert(string.end(), string_1.begin(), string_1.end());
    string.insert(string.end(), string_2.begin(), string_2.end());

    rle::RLEString::RLEncoder& encoder_1 = encoders.get_encoder(0);
    for (auto e : string_1) { encoder_1(e); }

    rle::RLEString::RLEncoder& encoder_2 = encoders.get_encoder(1);
    for (auto e : string_2) { encoder_2(e); }

    encoders.close();

    rle::RLEString rle_string;
    rle_string.load(testfiles_dir + "/test_cnct.seq");

    REQUIRE(string.size() == rle_string.size());
    for (std::size_t i = 0; i < string.size(); i++)
    {
        REQUIRE(string[i] == rle_string[i]);
    }
}

TEST_CASE("RLEString", "FromConcatNoRunInterruption")
{
    std::string path = testfiles_dir + "/test_cnct.seq";
    rle::RLEString::RLEncoderMerger encoders(path, 2);

    rle::string_type string_1 = {'A','A','A','G','C','A','A','T','T','T','T','T','T','A','A','G','G','A','A','A'};
    rle::string_type string_2 = {'A','A','A','G','C','A','A','T','T','T','T','T','T','A','A','G','G','A','A','A'};

    rle::string_type string;
    string.reserve(string_1.size() + string_2.size());
    string.insert(string.end(), string_1.begin(), string_1.end());
    string.insert(string.end(), string_2.begin(), string_2.end());

    rle::RLEString::RLEncoder& encoder_1 = encoders.get_encoder(0);
    for (auto e : string_1)
    { encoder_1(e); }

    rle::RLEString::RLEncoder& encoder_2 = encoders.get_encoder(1);
    for (auto e : string_2)
    { encoder_2(e); }

    encoders.close();

    rle::RLEString rle_string;
    rle_string.load(testfiles_dir + "/test_cnct.seq");

    REQUIRE(string.size() == rle_string.size());
    for (std::size_t i = 0; i < string.size(); i++)
    {
        REQUIRE(string[i] == rle_string[i]);
    }
}

TEST_CASE("RLEString", "RandomConcat")
{
    for (rle::size_type r = 0; r < 10; r++)
    {
        std::string path = testfiles_dir + "/rnd_cnct.seq";
        rle::RLEString::RLEncoderMerger encoders(path, 2);

        rle::string_type string_1 = random_string(1000);
        rle::string_type string_2 = random_string(100);

        rle::string_type string;
        string.reserve(string_1.size() + string_2.size());
        string.insert(string.end(), string_1.begin(), string_1.end());
        string.insert(string.end(), string_2.begin(), string_2.end());

        rle::RLEString::RLEncoder& encoder_1 = encoders.get_encoder(0);
        for (auto e : string_1)
        { encoder_1(e); }

        rle::RLEString::RLEncoder& encoder_2 = encoders.get_encoder(1);
        for (auto e : string_2)
        { encoder_2(e); }

        encoders.close();

        rle::RLEString rle_string;
        rle_string.load(testfiles_dir + "/rnd_cnct.seq");

        REQUIRE(string.size() == rle_string.size());
        for (std::size_t i = 0; i < string.size(); i++)
        {
            REQUIRE(string[i] == rle_string[i]);
        }
    }

}

TEST_CASE("RLEString", "RunCache_1")
{
    rle::string_type string = {'A','A','A','G','C','A','A','T','T','T','T','T','T','A','A','G','G','A','A','A'};
    rle::RLEString rle_string(string);

    rle::RLEString::RunCache cache(rle_string);

    for (std::size_t i = 0; i < string.size(); i++)
    {
        REQUIRE(string[i] == cache[i]);
    }
}

TEST_CASE("RLEString", "RunCache_2")
{
    rle::string_type string = {'A', 'G', 'C', 'A', 'T', 'A', 'G', 'A'};
    rle::RLEString rle_string(string);

    rle::RLEString::RunCache cache(rle_string);

    for (std::size_t i = 0; i < string.size(); i++)
    {
        REQUIRE(string[i] == cache[i]);
    }
}

//------------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
    Catch::Session session;

    using namespace Catch::Clara;

    auto cli = session.cli() |
               Opt( testfiles_dir, "dir" ) ["--test-dir"] ("specify the directory containing the test dna sequences files");

    session.cli(cli);

    int returnCode = session.applyCommandLine(argc, argv);

    if( returnCode != 0 ) return returnCode;

    session.run();
}