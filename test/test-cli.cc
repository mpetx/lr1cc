
#include <gtest/gtest.h>

#include "cli.hh"

using namespace lr1cc;

TEST(CLI, OK)
{
    std::vector<std::string> argv1 {
        "lr1cc",
        "amethyst.y"
    };
    auto conf1 = parse_argv(argv1);
    EXPECT_TRUE(conf1.has_value());
    EXPECT_EQ("amethyst.y", conf1.value().input_file);
    EXPECT_EQ("amethyst.y.csv", conf1.value().output_file);
    EXPECT_FALSE(conf1.value().help);

    std::vector<std::string> argv2 {
        "lr1cc",
        "-o",
        "cobra.csv",
        "cobra.grammar"
    };
    auto conf2 = parse_argv(argv2);
    EXPECT_TRUE(conf2.has_value());
    EXPECT_EQ("cobra.grammar", conf2.value().input_file);
    EXPECT_EQ("cobra.csv", conf2.value().output_file);
    EXPECT_FALSE(conf2.value().help);

    std::vector<std::string> argv3 {
        "lr1cc",
        "-h"
    };
    auto conf3 = parse_argv(argv3);
    EXPECT_TRUE(conf3.has_value());
    EXPECT_TRUE(conf3.value().help);
}

TEST(CLI, NG)
{
    std::vector<std::string> argv1 {
        "lr1cc",
        "-o"
    };
    auto conf1 = parse_argv(argv1);
    EXPECT_FALSE(conf1.has_value());

    std::vector<std::string> argv2 {
        "lr1cc",
        "-a",
        "gluttony.y"
    };
    auto conf2 = parse_argv(argv2);
    EXPECT_FALSE(conf2.has_value());
    
    std::vector<std::string> argv3 {
        "lr1cc",
        "greed.y",
        "sloth.y"
    };
    auto conf3 = parse_argv(argv3);
    EXPECT_FALSE(conf2.has_value());
}
