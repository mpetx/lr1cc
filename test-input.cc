
#include <gtest/gtest.h>

#include "input-lexer.hh"

#include <sstream>

using namespace lr1cc;

static void expect_next_token(Lexer &lexer, TokenType type, const std::string &value, std::size_t line)
{
    Token tok = lexer.front();
    EXPECT_EQ(type, tok.type);
    EXPECT_EQ(value, tok.value);
    EXPECT_EQ(line, lexer.line());
    lexer.pop();
}

static void expect_next_token(Lexer &lexer, TokenType type, std::size_t line)
{
    Token tok = lexer.front();
    EXPECT_EQ(type, tok.type);
    EXPECT_EQ(line, lexer.line());
    lexer.pop();
}

TEST(Lexer, Fundamental)
{
    std::istringstream in {
        "%start %end %terminal %intermediate %grammar\n"
        "S x foo_bar foo-bar foo.bar\n"
        "# this is a comment.\n"
        ": | ; [ ]\n"
    };

    Lexer lexer { in };

    expect_next_token(lexer, TokenType::start_marker, 1);
    expect_next_token(lexer, TokenType::end_marker, 1);
    expect_next_token(lexer, TokenType::terminal_marker, 1);
    expect_next_token(lexer, TokenType::intermediate_marker, 1);
    expect_next_token(lexer, TokenType::grammar_marker, 1);
    
    expect_next_token(lexer, TokenType::ident, "S", 2);
    expect_next_token(lexer, TokenType::ident, "x", 2);
    expect_next_token(lexer, TokenType::ident, "foo_bar", 2);
    expect_next_token(lexer, TokenType::ident, "foo-bar", 2);
    expect_next_token(lexer, TokenType::ident, "foo.bar", 2);

    expect_next_token(lexer, TokenType::colon, 4);
    expect_next_token(lexer, TokenType::bar, 4);
    expect_next_token(lexer, TokenType::semicolon, 4);
    expect_next_token(lexer, TokenType::square_start, 4);
    expect_next_token(lexer, TokenType::square_end, 4);

    expect_next_token(lexer, TokenType::end, 5);
}
