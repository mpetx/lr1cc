
#include <gtest/gtest.h>

#include "grammar.hh"

using namespace lr1cc;

TEST(Grammar, StartEnd)
{
    SymbolManager manager;
    auto start = manager.create_symbol("S", SymbolType::intermediate);
    auto end = manager.create_symbol("end", SymbolType::terminal);
    
    Grammar grammar;

    grammar.set_start(start);
    grammar.set_end(end);

    EXPECT_EQ(start, grammar.start());
    EXPECT_EQ(end, grammar.end());
}

TEST(Grammar, CalculateNullable)
{
    // S -> A B
    // T -> A B y
    // A -> x
    // A ->
    // B ->
    Symbol s { "S", SymbolType::intermediate };
    Symbol t { "T", SymbolType::intermediate };
    Symbol a { "A", SymbolType::intermediate };
    Symbol b { "B", SymbolType::intermediate };
    Symbol x { "x", SymbolType::terminal };
    Symbol y { "y", SymbolType::terminal };

    Production p1 { "1", &s, std::vector { &a, &b } };
    Production p2 { "2", &t, std::vector { &a, &b, &y } };
    Production p3 { "3", &a, std::vector { &x } };
    Production p4 { "4", &a, std::vector<Symbol *> { } };
    Production p5 { "5", &b, std::vector<Symbol *> { } };

    Grammar g;

    g.productions().push_back(&p1);
    g.productions().push_back(&p2);
    g.productions().push_back(&p3);
    g.productions().push_back(&p4);
    g.productions().push_back(&p5);

    g.calculate();

    EXPECT_TRUE(s.is_nullable());
    EXPECT_FALSE(t.is_nullable());
    EXPECT_TRUE(a.is_nullable());
    EXPECT_TRUE(b.is_nullable());
    
    EXPECT_FALSE(x.is_nullable());
    EXPECT_FALSE(y.is_nullable());
}

TEST(Grammar, CalculateFirst)
{
    // S -> A B z
    // A -> x
    // A ->
    // B -> y
    Symbol s { "S", SymbolType::intermediate };
    Symbol a { "A", SymbolType::intermediate };
    Symbol b { "B", SymbolType::intermediate };
    Symbol x { "x", SymbolType::terminal };
    Symbol y { "y", SymbolType::terminal };
    Symbol z { "z", SymbolType::terminal };    

    Production p1 { "1", &s, std::vector { &a, &b, &z } };
    Production p2 { "2", &a, std::vector { &x } };
    Production p3 { "3", &a, std::vector<Symbol *> { } };
    Production p4 { "4", &b, std::vector { &y } };

    Grammar g;

    g.productions().push_back(&p1);
    g.productions().push_back(&p2);
    g.productions().push_back(&p3);
    g.productions().push_back(&p4);

    g.calculate();

    EXPECT_EQ((std::set { &x, &y }), s.first());
    EXPECT_EQ((std::set { &x }), a.first());
    EXPECT_EQ((std::set { &y }), b.first());
}
