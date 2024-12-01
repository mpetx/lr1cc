
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
