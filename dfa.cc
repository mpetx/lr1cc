
#include "dfa.hh"

namespace lr1cc
{

    DFA::DFA()
        : m_start { nullptr }
    {
    }

    DFA::DFA(DFA &&dfa)
        : m_start { dfa.m_start },
          m_states { std::move(dfa.m_states) }
    {
        dfa.m_start = nullptr;
    }

    DFA &DFA::operator=(DFA &&dfa)
    {
        m_start = dfa.m_start;
        m_states = std::move(dfa.m_states);

        dfa.m_start = nullptr;
        
        return *this;
    }
    
}
