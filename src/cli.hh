#ifndef LR1CC_INCLUDE_CLI_HH
#define LR1CC_INCLUDE_CLI_HH

#include <optional>
#include <string>
#include <vector>

namespace lr1cc
{

    struct Config
    {
        std::string input_file;
        std::string output_file;
        bool help;
    };

    std::optional<Config> parse_argv(const std::vector<std::string> &argv);
    
}

#endif
