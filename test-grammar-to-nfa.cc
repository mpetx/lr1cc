
#include <gtest/gtest.h>

#include "grammar.hh"
#include "nfa.hh"
#include <iostream>
#include <algorithm>

using namespace lr1cc;

static bool nfa_result_accepts(const std::set<NFAState *> &result)
{
    return std::ranges::any_of(
        result,
        [](NFAState *s) {
            return s->acceptance().type == AcceptanceType::accept;
        });
}

static bool nfa_result_reduces(const std::set<NFAState *> &result, Production *p)
{
    return std::ranges::any_of(
        result,
        [=](NFAState *s) {
            return s->acceptance().type == AcceptanceType::reduce
                && s->acceptance().production == p;
        });
}

TEST(GrammarToNFA, NonLALR)
{
    // S -> a E c
    // S -> a F d
    // S -> b F c
    // S -> b E d
    // E -> x
    // F -> x
    Symbol s { "S", SymbolType::intermediate };
    Symbol e { "E", SymbolType::intermediate };
    Symbol f { "F", SymbolType::intermediate };
    Symbol a { "a", SymbolType::terminal };
    Symbol b { "b", SymbolType::terminal };
    Symbol c { "c", SymbolType::terminal };
    Symbol d { "d", SymbolType::terminal };
    Symbol x { "x", SymbolType::terminal };
    Symbol end { "end", SymbolType::terminal };

    Production p1 { "1", &s, std::vector { &a, &e, &c } };
    Production p2 { "2", &s, std::vector { &a, &f, &d } };
    Production p3 { "3", &s, std::vector { &b, &f, &c } };
    Production p4 { "4", &s, std::vector { &b, &e, &d } };
    Production p5 { "5", &e, std::vector { &x } };
    Production p6 { "6", &f, std::vector { &x } };

    Grammar g;
    g.set_start(&s);
    g.set_end(&end);

    g.productions().push_back(&p1);
    g.productions().push_back(&p2);
    g.productions().push_back(&p3);
    g.productions().push_back(&p4);
    g.productions().push_back(&p5);
    g.productions().push_back(&p6);

    g.calculate();

    NFA nfa = grammar_to_nfa(g);

    EXPECT_TRUE(nfa_result_accepts(nfa.run(std::vector { &s, &end })));
    EXPECT_TRUE(nfa_result_reduces(nfa.run(std::vector { &a, &e, &c, &end }), &p1));
    EXPECT_TRUE(nfa_result_reduces(nfa.run(std::vector { &a, &f, &d, &end }), &p2));
    EXPECT_TRUE(nfa_result_reduces(nfa.run(std::vector { &b, &f, &c, &end }), &p3));
    EXPECT_TRUE(nfa_result_reduces(nfa.run(std::vector { &b, &e, &d, &end }), &p4));
    EXPECT_TRUE(nfa_result_reduces(nfa.run(std::vector { &a, &x, &c }), &p5));
    EXPECT_TRUE(nfa_result_reduces(nfa.run(std::vector { &a, &x, &d }), &p6));
    EXPECT_TRUE(nfa_result_reduces(nfa.run(std::vector { &b, &x, &c }), &p6));
    EXPECT_TRUE(nfa_result_reduces(nfa.run(std::vector { &b, &x, &d }), &p5));

    EXPECT_FALSE(nfa_result_reduces(nfa.run(std::vector { &a, &x, &c }), &p6));
    EXPECT_FALSE(nfa_result_reduces(nfa.run(std::vector { &a, &x, &d }), &p5));
    EXPECT_FALSE(nfa_result_reduces(nfa.run(std::vector { &b, &x, &c }), &p5));
    EXPECT_FALSE(nfa_result_reduces(nfa.run(std::vector { &b, &x, &d }), &p6));
}
