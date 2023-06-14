//
//  rleinfo.cpp
//

#include <CLI/CLI.hpp>
#include <rle/rle_string.hpp>

int main(int argc, char **argv)
{
    CLI::App app("rleinfo");
    
    std::vector<std::string> rle_string_paths;
    bool load_string = false;
    
    app.add_option("input", rle_string_paths, "List of RLE strings.")->allow_extra_args(true)->delimiter(',')->configurable()->required()->check(CLI::ExistingFile);
    app.add_flag("-l,--load-string", load_string)->configurable();
    app.set_config("--configure");
    app.allow_windows_style_options();
    
    CLI11_PARSE(app, argc, argv);
    
    // open each rle_string and output statistics
    std::cout << "name,n,r" << std::endl;
    for (auto& rle_string_path : rle_string_paths)
    {
        if (load_string)
        {
            rle::RLEString rle_string;
            rle_string.load(rle_string_path);
            std::cout << rle_string_path << "," << rle_string.size() << "," << rle_string.number_of_runs() << std::endl;
        }
        else
        {
            rle::RLEString::Metadata rle_metadata(rle_string_path + ".meta", rle::RLEString::Metadata::read_tag());
            std::cout << rle_string_path << "," << rle_metadata.size << "," << rle_metadata.runs << std::endl;
        }
    }
}