
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

    EXPECT_THROW(x.set_nullable(), std::runtime_error);

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
}
