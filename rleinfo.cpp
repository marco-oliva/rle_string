//
//  rleinfo.cpp
//

#include <CLI/CLI.hpp>
#include <rle/rle_string.hpp>

int main(int argc, char **argv)
{
    CLI::App app("rleinfo");
    
    std::vector<std::string> rle_string_paths;
    
    app.add_option("input", rle_string_paths, "List of RLE strings.")->allow_extra_args(true)->delimiter(',')->configurable()->required()->check(CLI::ExistingFile);
    app.set_config("--configure");
    app.allow_windows_style_options();
    
    CLI11_PARSE(app, argc, argv);
    
    // open each rle_string and output statistics
    std::cout << "name,n,r" << std::endl;
    for (auto& rle_string_path : rle_string_paths)
    {
        rle::RLEString::Metadata rle_metadata(rle_string_path + ".meta", rle::RLEString::Metadata::read_tag());
        std::cout << rle_string_path << "," << rle_metadata.size << "," << rle_metadata.runs << std::endl;
    }
}