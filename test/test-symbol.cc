
#include <gtest/gtest.h>

#include "symbol.hh"

#include <stdexcept>

using namespace lr1cc;

TEST(Symbol, Fundamental)
{
    Symbol x { "x", SymbolType::terminal };
    Symbol s { "S", SymbolType::intermediate };

    EXPECT_EQ("x", x.name());
    EXPECT_EQ("S", s.name());

    EXPECT_EQ(SymbolType::terminal, x.type());
    EXPECT_EQ(SymbolType::intermediate, s.type());

    EXPECT_TRUE(x.is_terminal());
    EXPECT_FALSE(s.is_terminal());

    EXPECT_FALSE(x.is_intermediate());
    EXPECT_TRUE(s.is_intermediate());

    EXPECT_FALSE(x.is_nullable());
    EXPECT_FALSE(s.is_nullable());

    EXPECT_TRUE(x.first().contains(&x));
    EXPECT_FALSE(s.first().contains(&s));

    s.set_nullable();

    EXPECT_TRUE(s.is_nullable());
}

TEST(SymbolManager, Fundamental)
{
    SymbolManager manager;

    auto x = manager.create_symbol("x", SymbolType::terminal);
    auto s = manager.create_symbol("S", SymbolType::intermediate);

    EXPECT_EQ("x", x->name());
    EXPECT_EQ("S", s->name());

    EXPECT_TRUE(x->is_terminal());
    EXPECT_TRUE(s->is_intermediate());

    EXPECT_EQ(nullptr, manager.create_symbol("x", SymbolType::terminal));
    EXPECT_EQ(nullptr, manager.create_symbol("x", SymbolType::intermediate));
    EXPECT_EQ(nullptr, manager.create_symbol("S", SymbolType::terminal));
    EXPECT_EQ(nullptr, manager.create_symbol("S", SymbolType::intermediate));
    
    EXPECT_EQ(x, manager.get_symbol("x"));
    EXPECT_EQ(s, manager.get_symbol("S"));

    EXPECT_EQ(nullptr, manager.get_symbol("y"));

    std::vector<Symbol *> symbols;
    for (Symbol *a : manager.symbols())
    {
        symbols.push_back(a);
    }
    EXPECT_EQ((std::vector { x, s }), symbols);
}

TEST(Symbols, IsNullable)
{
    Symbol a { "A", SymbolType::intermediate };
    Symbol b { "B", SymbolType::intermediate };
    Symbol c { "C", SymbolType::intermediate };

    a.set_nullable();
    b.set_nullable();

    EXPECT_TRUE(is_nullable(std::vector { &a, &b }));
    EXPECT_FALSE(is_nullable(std::vector { &a, &c }));
}

TEST(Symbols, First)
{
    Symbol a { "A", SymbolType::intermediate };
    Symbol b { "B", SymbolType::intermediate };
    Symbol c { "C", SymbolType::intermediate };
    
    Symbol x { "x", SymbolType::terminal };
    Symbol y { "y", SymbolType::terminal };
    Symbol z { "z", SymbolType::terminal };

    a.set_nullable();
    b.set_nullable();

    a.first().emplace(&x);
    b.first().emplace(&y);
    c.first().emplace(&y);

    std::set<Symbol *> expect_abz { &x, &y, &z };
    EXPECT_EQ(expect_abz, first(std::vector { &a, &b, &z }));
    EXPECT_EQ(expect_abz, first(std::vector { &a, &b }, &z));

    std::set<Symbol *> expect_acz { &x, &y };
    EXPECT_EQ(expect_acz, first(std::vector { &a, &c, &z }));
    EXPECT_EQ(expect_acz, first(std::vector { &a, &c }, &z));
}
