
#include "dfa.hh"
#include "util.hh"

#include <unordered_map>

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

    using NFAStatesDFAStateAssociation = std::unordered_map<std::set<NFAState *>, DFAState *, SetHash<NFAState *>>;
    
    static std::set<Symbol *> nfa_states_inputs(const std::set<NFAState *> &nstates)
    {
        std::set<Symbol *> inputs;

        for (NFAState *state : nstates)
        {
            for (const auto &pair : state->transitions())
            {
                inputs.emplace(pair.first);
            }
        }

        inputs.erase(nullptr);
        
        return inputs;
    }

    static DFAState *get_dfa_state(const std::set<NFAState *> &nstates, DFA &dfa, NFAStatesDFAStateAssociation &nfa_to_dfa)
    {
        auto iter = nfa_to_dfa.find(nstates);

        if (iter == nfa_to_dfa.end())
        {
            auto dstate = dfa.create_state(nstates);

            nfa_to_dfa.emplace(nstates, dstate);

            auto inputs = nfa_states_inputs(nstates);

            for (Symbol *input : inputs)
            {
                auto to_nstates = transit(nstates, input);
                    
                auto to_dstate = get_dfa_state(to_nstates, dfa, nfa_to_dfa);
                    
                dstate->transitions().emplace(input, to_dstate);
            }

            return dstate;
        }
        else
        {
            return iter->second;
        }
    }
    
    DFA nfa_to_dfa(const NFA &nfa)
    {
        DFA dfa;
        
        NFAStatesDFAStateAssociation nfa_to_dfa;

        std::set initial_nstates { nfa.start() };
        epsilon_close(initial_nstates);
        
        auto initial_dstate = get_dfa_state(initial_nstates, dfa, nfa_to_dfa);

        dfa.set_start(initial_dstate);

        return dfa;
    }
    
}
