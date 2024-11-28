
#include <gtest/gtest.h>

#include "conflict.hh"

using namespace lr1cc;

TEST(Conflict, ReduceReduce)
{
    Symbol s { "S", SymbolType::intermediate };
    Symbol x { "x", SymbolType::terminal };
    Symbol y { "y", SymbolType::terminal };
    
    Production p1 { "1", &s, std::vector { &x } };
    Production p2 { "2", &s, std::vector { &x, &x } };
    
    NFAState n1 { Acceptance { AcceptanceType::accept, nullptr } };
    NFAState n2 { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState n3 { Acceptance { AcceptanceType::reduce, &p1 } };
    NFAState n4 { Acceptance { AcceptanceType::reduce, &p2 } };

    DFAState d1 { std::vector { &n2 } };
    DFAState d2 { std::vector { &n1, &n3 } };
    DFAState d3 { std::vector { &n2 } };
    DFAState d4 { std::vector { &n3, &n4 } };

    d1.transitions().emplace(&x, &d2);
    d1.transitions().emplace(&y, &d3);
    d3.transitions().emplace(&x, &d4);

    DFA dfa;
    dfa.set_start(&d1);
    
    auto conflicts = collect_conflicts(dfa);

    EXPECT_EQ(2, conflicts.size());

    EXPECT_EQ(&d2, conflicts.at(0).first_state);
    EXPECT_EQ(&d2, conflicts.at(0).second_state);
    EXPECT_EQ((std::vector { &x }), conflicts.at(0).start_to_first);
    EXPECT_EQ(std::vector<Symbol *> { }, conflicts.at(0).first_to_second);

    EXPECT_EQ(&d4, conflicts.at(1).first_state);
    EXPECT_EQ(&d4, conflicts.at(1).second_state);
    EXPECT_EQ((std::vector { &y, &x }), conflicts.at(1).start_to_first);
    EXPECT_EQ(std::vector<Symbol *> { }, conflicts.at(1).first_to_second);
}

TEST(Conflict, ShiftReduce)
{
    Symbol s { "S", SymbolType::intermediate };
    Symbol x { "x", SymbolType::terminal };
    Symbol y { "y", SymbolType::terminal };
    
    Production p1 { "1", &s, std::vector { &x } };
    
    NFAState n1 { Acceptance { AcceptanceType::accept, nullptr } };
    NFAState n2 { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState n3 { Acceptance { AcceptanceType::reduce, &p1 } };

    DFAState d1 { std::vector { &n1 } };
    DFAState d2 { std::vector { &n2 } };
    DFAState d3 { std::vector { &n3 } };

    d1.transitions().emplace(&x, &d2);
    d2.transitions().emplace(&y, &d3);

    DFA dfa;
    dfa.set_start(&d1);

    auto conflicts = collect_conflicts(dfa);

    EXPECT_EQ(1, conflicts.size());
    
    EXPECT_EQ(&d1, conflicts.at(0).first_state);
    EXPECT_EQ(&d3, conflicts.at(0).second_state);
    EXPECT_EQ(std::vector<Symbol *> { }, conflicts.at(0).start_to_first);
    EXPECT_EQ((std::vector { &x, &y }), conflicts.at(0).first_to_second);
}
