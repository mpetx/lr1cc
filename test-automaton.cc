
#include <gtest/gtest.h>

#include "grammar.hh"
#include "nfa.hh"
#include "dfa.hh"

#include <algorithm>

using namespace lr1cc;

static bool nfa_result_accepts(const std::set<NFAState *> &result)
{
    auto accepts = [](NFAState *s) {
        return s->acceptance().type == AcceptanceType::accept;
    };
    
    auto accepts_or_rejects = [&](NFAState *s) {
        return s->acceptance().type == AcceptanceType::reject
            || accepts(s);
    };
    
    return std::ranges::all_of(result, accepts_or_rejects)
        && std::ranges::any_of(result, accepts);
}

static bool nfa_result_rejects(const std::set<NFAState *> &result)
{
    auto rejects = [](NFAState *s) {
        return s->acceptance().type == AcceptanceType::reject;
    };

    return std::ranges::all_of(result, rejects);
}

static bool nfa_result_reduces(const std::set<NFAState *> &result, Production *p)
{
    auto reduces = [&](NFAState *s) {
        return s->acceptance().type == AcceptanceType::reduce
            && s->acceptance().production == p;
    };
    
    auto reduces_or_rejects = [&](NFAState *s) {
        return s->acceptance().type == AcceptanceType::reject
            || reduces(s);
    };

    return std::ranges::all_of(result, reduces_or_rejects)
        && std::ranges::any_of(result, reduces);
}

static bool dfa_result_accepts(DFAState *s)
{
    return s != nullptr && s->accepts() && s->reductions().empty();
}

static bool dfa_result_reduces(DFAState *s, Production *p)
{
    return s != nullptr && !s->accepts() && s->reductions().size() == 1 && s->reductions().contains(p);
}

static bool dfa_result_rejects(DFAState *s)
{
    return s == nullptr || !s->accepts() && s->reductions().empty();
}

TEST(Automata, NonLALR)
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
    DFA dfa = nfa_to_dfa(nfa);

    std::vector s_end { &s, &end };
    EXPECT_TRUE(nfa_result_accepts(nfa.run(s_end)));
    EXPECT_TRUE(dfa_result_accepts(dfa.run(s_end)));

    std::vector aec_end { &a, &e, &c, &end };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(aec_end), &p1));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(aec_end), &p1));

    std::vector afd_end { &a, &f, &d, &end };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(afd_end), &p2));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(afd_end), &p2));

    std::vector bfc_end { &b, &f, &c, &end };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(bfc_end), &p3));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(bfc_end), &p3));

    std::vector bed_end { &b, &e, &d, &end };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(bed_end), &p4));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(bed_end), &p4));

    std::vector axc { &a, &x, &c };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(axc), &p5));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(axc), &p5));

    std::vector axd { &a, &x, &d };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(axd), &p6));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(axd), &p6));

    std::vector bxc { &b, &x, &c };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(bxc), &p6));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(bxc), &p6));

    std::vector bxd { &b, &x, &d };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(bxd), &p5));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(bxd), &p5));

    std::vector axc_end { &a, &x, &c, &end };
    EXPECT_TRUE(nfa_result_rejects(nfa.run(axc_end)));
    EXPECT_TRUE(dfa_result_rejects(dfa.run(axc_end)));

    std::vector ax { &a, &x };
    EXPECT_TRUE(nfa_result_rejects(nfa.run(ax)));
    EXPECT_TRUE(dfa_result_rejects(dfa.run(ax)));

    EXPECT_TRUE(nfa_result_rejects(nfa.run(std::ranges::views::single(&a))));
    EXPECT_TRUE(dfa_result_rejects(dfa.run(std::ranges::views::single(&a))));

    EXPECT_TRUE(nfa_result_rejects(nfa.run(std::ranges::views::empty<Symbol *>)));
    EXPECT_TRUE(dfa_result_rejects(dfa.run(std::ranges::views::empty<Symbol *>)));
}
