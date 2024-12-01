
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

    std::vector ax_end { &a, &x, &end };
    EXPECT_TRUE(nfa_result_rejects(nfa.run(ax_end)));
    EXPECT_TRUE(dfa_result_rejects(dfa.run(ax_end)));
    
    std::vector ax { &a, &x };
    EXPECT_TRUE(nfa_result_rejects(nfa.run(ax)));
    EXPECT_TRUE(dfa_result_rejects(dfa.run(ax)));

    EXPECT_TRUE(nfa_result_rejects(nfa.run(std::ranges::views::single(&a))));
    EXPECT_TRUE(dfa_result_rejects(dfa.run(std::ranges::views::single(&a))));

    EXPECT_TRUE(nfa_result_rejects(nfa.run(std::ranges::views::empty<Symbol *>)));
    EXPECT_TRUE(dfa_result_rejects(dfa.run(std::ranges::views::empty<Symbol *>)));
}

TEST(Automata, NullableTreatment)
{
    // S -> T X Y
    // T -> a
    // X -> b
    // X ->
    // Y -> c
    // Y ->
    Symbol s { "S", SymbolType::intermediate };
    Symbol t { "T", SymbolType::intermediate };
    Symbol x { "X", SymbolType::intermediate };
    Symbol y { "Y", SymbolType::intermediate };
    Symbol a { "a", SymbolType::terminal };
    Symbol b { "b", SymbolType::terminal };
    Symbol c { "c", SymbolType::terminal };
    Symbol end { "end", SymbolType::terminal };

    Production p1 { "p1", &s, std::vector { &t, &x, &y } };
    Production p2 { "p2", &t, std::vector { &a } };
    Production p3 { "p3", &x, std::vector { &b } };
    Production p4 { "p4", &x, std::vector<Symbol *> { } };
    Production p5 { "p5", &y, std::vector { &c } };
    Production p6 { "p6", &y, std::vector<Symbol *> { } };

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
    
    std::vector txy_end { &t, &x, &y, &end };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(txy_end), &p1));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(txy_end), &p1));    

    std::vector txc_end { &t, &x, &c, &end };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(txc_end), &p5));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(txc_end), &p5));    

    std::vector tbc { &t, &b, &c };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(tbc), &p3));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(tbc), &p3));

    std::vector ab { &a, &b };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(ab), &p2));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(ab), &p2));

    std::vector tc { &t, &c };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(tc), &p4));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(tc), &p4));

    std::vector ac { &a, &c };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(ac), &p2));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(ac), &p2));

    std::vector tx_end { &t, &x, &end };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(tx_end), &p6));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(tx_end), &p6));

    std::vector tb_end { &t, &b, &end };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(tb_end), &p3));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(tb_end), &p3));

    std::vector a_end { &a, &end };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(a_end), &p2));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(a_end), &p2));

    std::vector ax { &a, &x };
    EXPECT_TRUE(nfa_result_rejects(nfa.run(ax)));
    EXPECT_TRUE(dfa_result_rejects(dfa.run(ax)));

    std::vector axy_end { &a, &x, &y, &end };
    EXPECT_TRUE(nfa_result_rejects(nfa.run(axy_end)));
    EXPECT_TRUE(dfa_result_rejects(dfa.run(axy_end)));

    EXPECT_TRUE(nfa_result_rejects(nfa.run(std::ranges::views::single(&a))));
    EXPECT_TRUE(dfa_result_rejects(dfa.run(std::ranges::views::single(&a))));
    
    EXPECT_TRUE(nfa_result_rejects(nfa.run(std::ranges::views::single(&t))));
    EXPECT_TRUE(dfa_result_rejects(dfa.run(std::ranges::views::single(&t))));
}

TEST(Automate, Recursive)
{
    // S -> a S c
    // S -> T
    // T -> b T c
    // T ->
    Symbol s { "S", SymbolType::intermediate };
    Symbol t { "T", SymbolType::intermediate };
    Symbol a { "a", SymbolType::terminal };
    Symbol b { "b", SymbolType::terminal };
    Symbol c { "c", SymbolType::terminal };
    Symbol end { "end", SymbolType::terminal };

    Production p1 { "p1", &s, std::vector { &a, &s, &c } };
    Production p2 { "p2", &s, std::vector { &t } };
    Production p3 { "p3", &t, std::vector { &b, &t, &c } };
    Production p4 { "p4", &t, std::vector<Symbol *> { } };

    Grammar g;
    g.set_start(&s);
    g.set_end(&end);

    g.productions().push_back(&p1);
    g.productions().push_back(&p2);
    g.productions().push_back(&p3);
    g.productions().push_back(&p4);

    g.calculate();

    NFA nfa = grammar_to_nfa(g);
    DFA dfa = nfa_to_dfa(nfa);

    std::vector s_end { &s, &end };
    EXPECT_TRUE(nfa_result_accepts(nfa.run(s_end)));
    EXPECT_TRUE(dfa_result_accepts(dfa.run(s_end)));

    std::vector asc_end { &a, &s, &c, &end };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(asc_end), &p1));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(asc_end), &p1));

    std::vector ascc { &a, &s, &c, &c };
    EXPECT_TRUE(nfa_result_rejects(nfa.run(ascc)));
    EXPECT_TRUE(dfa_result_rejects(dfa.run(ascc)));

    std::vector aascc { &a, &a, &s, &c, &c };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(aascc), &p1));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(aascc), &p1));

    std::vector aasc_end { &a, &a, &s, &c, &end };
    EXPECT_TRUE(nfa_result_rejects(nfa.run(aasc_end)));
    EXPECT_TRUE(dfa_result_rejects(dfa.run(aasc_end)));

    std::vector aatc { &a, &a, &t, &c };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(aatc), &p2));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(aatc), &p2));

    std::vector aat_end { &a, &a, &t, &end };
    EXPECT_TRUE(nfa_result_rejects(nfa.run(aat_end)));
    EXPECT_TRUE(dfa_result_rejects(dfa.run(aat_end)));

    std::vector aabtcc { &a, &a, &b, &t, &c, &c };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(aabtcc), &p3));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(aabtcc), &p3));

    std::vector aabc { &a, &a, &b, &c };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(aabc), &p4));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(aabc), &p4));

    std::vector t_end { &t, &end };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(t_end), &p2));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(t_end), &p2));

    std::vector btc_end { &b, &t, &c, &end };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(btc_end), &p3));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(btc_end), &p3));

    std::vector btcc { &b, &t, &c, &c };
    EXPECT_TRUE(nfa_result_rejects(nfa.run(btcc)));
    EXPECT_TRUE(dfa_result_rejects(dfa.run(btcc)));

    std::vector bbtcc { &b, &b, &t, &c, &c};
    EXPECT_TRUE(nfa_result_reduces(nfa.run(bbtcc), &p3));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(bbtcc), &p3));

    std::vector bbtc_end { &b, &b, &t, &c, &end};
    EXPECT_TRUE(nfa_result_rejects(nfa.run(bbtc_end)));
    EXPECT_TRUE(dfa_result_rejects(dfa.run(bbtc_end)));
    
    std::vector bbc { &b, &b, &c };
    EXPECT_TRUE(nfa_result_reduces(nfa.run(bbc), &p4));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(bbc), &p4));

    EXPECT_TRUE(nfa_result_reduces(nfa.run(std::ranges::views::single(&end)), &p4));
    EXPECT_TRUE(dfa_result_reduces(dfa.run(std::ranges::views::single(&end)), &p4));
}
