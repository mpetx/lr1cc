
#include <gtest/gtest.h>

#include "dfa.hh"

using namespace lr1cc;

TEST(DFAState, Constructor)
{
    Symbol s { "S", SymbolType::intermediate };
    Symbol x { "S", SymbolType::terminal };
    Symbol y { "S", SymbolType::terminal };

    Production p1 { "1", &s, std::vector { &x } };
    Production p2 { "2", &s, std::vector { &y } };
    
    NFAState n1 { Acceptance { AcceptanceType::accept, nullptr } };
    NFAState n2 { Acceptance { AcceptanceType::reduce, &p1 } };
    NFAState n3 { Acceptance { AcceptanceType::reduce, &p2 } };
    NFAState n4 { Acceptance { AcceptanceType::reject, nullptr } };

    DFAState s1 { std::vector { &n1, &n2, &n3, &n4 } };
    DFAState s2 { std::vector { &n2, &n4 } };
    DFAState s3 { std::vector { &n4 } };

    EXPECT_TRUE(s1.accepts());
    EXPECT_FALSE(s2.accepts());
    EXPECT_FALSE(s3.accepts());
    
    EXPECT_EQ((std::set { &p1, &p2 }), s1.reductions());
    EXPECT_EQ((std::set { &p1 }), s2.reductions());
    EXPECT_TRUE(s3.reductions().empty());

    EXPECT_FALSE(s1.rejects());
    EXPECT_FALSE(s2.rejects());
    EXPECT_TRUE(s3.rejects());
}

TEST(DFA, Fundamental)
{
    NFAState n1 { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState n2 { Acceptance { AcceptanceType::accept, nullptr } };
    
    DFA dfa;

    auto s1 = dfa.create_state(std::vector { &n1 });
    auto s2 = dfa.create_state(std::vector { &n2 });

    EXPECT_TRUE(s1->rejects());
    EXPECT_TRUE(s2->accepts());

    dfa.set_start(s1);

    EXPECT_EQ(s1, dfa.start());
}

TEST(DFA, Run)
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

    EXPECT_TRUE(dfa.run(std::vector { &s, &end })->accepts());
    EXPECT_TRUE(dfa.run(std::vector { &a, &e, &c, &end })->reductions().contains(&p1));
    EXPECT_TRUE(dfa.run(std::vector { &a, &f, &d, &end })->reductions().contains(&p2));
    EXPECT_TRUE(dfa.run(std::vector { &b, &f, &c, &end })->reductions().contains(&p3));
    EXPECT_TRUE(dfa.run(std::vector { &b, &e, &d, &end })->reductions().contains(&p4));
    EXPECT_TRUE(dfa.run(std::vector { &a, &x, &c })->reductions().contains(&p5));
    EXPECT_TRUE(dfa.run(std::vector { &a, &x, &d })->reductions().contains(&p6));
    EXPECT_TRUE(dfa.run(std::vector { &b, &x, &c })->reductions().contains(&p6));
    EXPECT_TRUE(dfa.run(std::vector { &b, &x, &d })->reductions().contains(&p5));

    EXPECT_FALSE(dfa.run(std::vector { &a, &x, &c })->reductions().contains(&p6));
    EXPECT_FALSE(dfa.run(std::vector { &a, &x, &d })->reductions().contains(&p5));
    EXPECT_FALSE(dfa.run(std::vector { &b, &x, &c })->reductions().contains(&p5));
    EXPECT_FALSE(dfa.run(std::vector { &b, &x, &d })->reductions().contains(&p6));
}
