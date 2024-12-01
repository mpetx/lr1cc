
#include "cli.hh"

namespace lr1cc
{

    std::optional<Config> parse_argv(const std::vector<std::string> &argv)
    {
        std::optional<std::string> output_file;

        std::size_t i = 1;

        while (i < argv.size())
        {
            if (argv[i] == "-o")
            {
                ++i;

                if (i >= argv.size())
                {
                    return std::nullopt;
                }

                output_file = argv[i];
            }
            else if (argv[i] == "-h" || argv[i] == "--help")
            {
                return Config { "", "", true };
            }
            else if (argv[i].starts_with("-"))
            {
                return std::nullopt;
            }
            else
            {
                break;
            }

            ++i;
        }

        if (i != argv.size() - 1)
        {
            return std::nullopt;
        }

        std::string input_file = argv[i];

        if (output_file.has_value())
        {
            return Config { input_file, output_file.value(), false };
        }
        else
        {
            std::string default_output_file { input_file };
            default_output_file.append(".csv");
            return Config { input_file, default_output_file, false };
        }
    }

    
}
