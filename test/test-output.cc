
#include <gtest/gtest.h>

#include "output.hh"

#include <sstream>

using namespace lr1cc;

TEST(Output, Fundamental)
{
    Symbol symbols[] = {
        { "x", SymbolType::terminal },
        { "y", SymbolType::terminal },
        { "S", SymbolType::intermediate }
    };

    Production p { "p", symbols + 2, std::vector { symbols + 0 } };

    NFAState n_reject { Acceptance { AcceptanceType::reject, nullptr } };
    NFAState n_accept { Acceptance { AcceptanceType::accept, nullptr } };
    NFAState n_reduce { Acceptance { AcceptanceType::reduce, &p } };
    
    DFA dfa;

    auto d_start = dfa.create_state(std::vector { &n_reject });
    auto d_mid_a = dfa.create_state(std::vector { &n_reject });
    auto d_a = dfa.create_state(std::vector { &n_accept });
    auto d_mid_r = dfa.create_state(std::vector { &n_reject });
    auto d_r = dfa.create_state(std::vector { &n_reduce });
    
    dfa.set_start(d_start);

    d_start->transitions().emplace(symbols + 0, d_mid_a);
    d_mid_a->transitions().emplace(symbols + 1, d_a);
    d_start->transitions().emplace(symbols + 2, d_mid_r);
    d_mid_r->transitions().emplace(symbols + 0, d_r);

    std::vector columns { symbols + 0, symbols + 1, symbols + 2 };

    std::ostringstream out { std::ios_base::binary };

    output_lr1_table(dfa, columns, out);

    auto result = out.str();
    
    std::string expect {
        ",x,y,S\r\n"
        "1,S2,,G3\r\n"
        "2,,A,\r\n"
        "3,Rp,,\r\n"
    };

    EXPECT_EQ(expect, result);
}
