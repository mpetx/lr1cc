
#include <gtest/gtest.h>

#include "nfa.hh"

using namespace lr1cc;

TEST(NFAState, Fundamental)
{
    Symbol s { "S", SymbolType::intermediate };
    Symbol x { "x", SymbolType::terminal };
    Production p { "0", &s, std::vector { &x } };
    
    NFAState s1 { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState s2 { Acceptance { AcceptanceType::accept, nullptr } };
    NFAState s3 { Acceptance { AcceptanceType::reduce, &p } };

    EXPECT_EQ(AcceptanceType::reject, s1.acceptance().type);
    EXPECT_EQ(AcceptanceType::accept, s2.acceptance().type);
    EXPECT_EQ(AcceptanceType::reduce, s3.acceptance().type);

    EXPECT_EQ(&p, s3.acceptance().production);
}

TEST(NFAState, AddTransition)
{
    Symbol x { "x", SymbolType::terminal };
    Symbol y { "y", SymbolType::terminal };

    NFAState s { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState s_x1 { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState s_x2 { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState s_y { Acceptance { AcceptanceType::reject, nullptr } };

    s.add_transition(&x, &s_x1);
    s.add_transition(&x, &s_x2);
    s.add_transition(&y, &s_y);

    EXPECT_EQ((std::set { &s_x1, &s_x2 }), s.transitions().find(&x)->second);
    EXPECT_EQ((std::set { &s_y }), s.transitions().find(&y)->second);
}

TEST(NFAState, EpsilonClose)
{
    NFAState s1 { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState s2 { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState s3 { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState s4 { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState s5 { Acceptance { AcceptanceType::reject, nullptr } };

    s1.add_transition(nullptr, &s2);
    s2.add_transition(nullptr, &s3);
    s3.add_transition(nullptr, &s1);
    s4.add_transition(nullptr, &s5);

    std::set result { &s1, &s4 };
    epsilon_close(result);
    std::set expect { &s1, &s2, &s3, &s4, &s5 };
    EXPECT_EQ(expect, result);
}

TEST(NFAState, Transit)
{
    Symbol x { "x", SymbolType::terminal };
    Symbol y { "y", SymbolType::terminal };
    
    NFAState s1 { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState s2 { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState s3 { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState s4 { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState s5 { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState t1 { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState t2 { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState t3 { Acceptance { AcceptanceType::reject, nullptr } };
    
    s1.add_transition(&x, &s2);
    s1.add_transition(&x, &s3);
    s1.add_transition(&y, &t1);
    s4.add_transition(&x, &s5);
    s4.add_transition(&y, &t2);
    t2.add_transition(nullptr, &t3);

    std::set states { &s1, &s4 };
    
    auto result_x = transit(states, &x);
    std::set expect_x { &s2, &s3, &s5 };
    EXPECT_EQ(expect_x, result_x);

    auto result_y = transit(states, &y);
    std::set expect_y { &t1, &t2, &t3 };
    EXPECT_EQ(expect_y, result_y);
}

TEST(NFA, Fundamental)
{
    NFA nfa;

    auto s1 = nfa.create_state(Acceptance { AcceptanceType::reject, nullptr });
    auto s2 = nfa.create_state(Acceptance { AcceptanceType::accept, nullptr });

    EXPECT_EQ(AcceptanceType::reject, s1->acceptance().type);
    EXPECT_EQ(AcceptanceType::accept, s2->acceptance().type);

    nfa.set_start(s1);

    EXPECT_EQ(s1, nfa.start());
}
