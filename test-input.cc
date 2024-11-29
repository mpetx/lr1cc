
#include <gtest/gtest.h>

#include "input-lexer.hh"
#include "input-parser.hh"

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

TEST(Parser, Fundamental)
{
    std::istringstream in {
        "%start S\n"
        "%end end\n"
        "%terminal x y\n"
        "%intermediate A B\n"
        "%grammar\n"
        "S: A [a]\n"
        " | B [b]\n"
        " ;\n"
        "A: x x [xx] ;\n"
        "B: y x [yx] ;\n"
    };

    SymbolManager manager;
    std::vector<std::unique_ptr<Production>> productions;

    Grammar g = parse_input(in, manager, productions);

    EXPECT_EQ("S", g.start()->name());
    EXPECT_EQ("end", g.end()->name());

    EXPECT_EQ(4, g.productions().size());

    EXPECT_EQ("a", g.productions().at(0)->name);
    EXPECT_EQ("S", g.productions().at(0)->lhs->name());
    EXPECT_EQ(1, g.productions().at(0)->rhs.size());
    EXPECT_EQ("A", g.productions().at(0)->rhs.at(0)->name());

    EXPECT_EQ("b", g.productions().at(1)->name);
    EXPECT_EQ("S", g.productions().at(1)->lhs->name());
    EXPECT_EQ(1, g.productions().at(1)->rhs.size());
    EXPECT_EQ("B", g.productions().at(1)->rhs.at(0)->name());

    EXPECT_EQ("xx", g.productions().at(2)->name);
    EXPECT_EQ("A", g.productions().at(2)->lhs->name());
    EXPECT_EQ(2, g.productions().at(2)->rhs.size());
    EXPECT_EQ("x", g.productions().at(2)->rhs.at(0)->name());
    EXPECT_EQ("x", g.productions().at(2)->rhs.at(1)->name());
    
    EXPECT_EQ("yx", g.productions().at(3)->name);
    EXPECT_EQ("B", g.productions().at(3)->lhs->name());
    EXPECT_EQ(2, g.productions().at(3)->rhs.size());
    EXPECT_EQ("y", g.productions().at(3)->rhs.at(0)->name());
    EXPECT_EQ("x", g.productions().at(3)->rhs.at(1)->name());
}
