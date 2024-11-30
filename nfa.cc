
#include "nfa.hh"
#include "util.hh"

#include <algorithm>
#include <deque>
#include <ranges>
#include <utility>
#include <unordered_map>

namespace lr1cc
{

    NFAState::NFAState(const Acceptance &acceptance)
        : m_acceptance { acceptance }
    {
    }

    void NFAState::add_transition(Symbol *input, NFAState *to_state)
    {
        auto iter = m_transitions.find(input);

        if (iter == m_transitions.end())
        {
            m_transitions.emplace(input, std::set { to_state });
        }
        else
        {
            iter->second.emplace(to_state);
        }
    }

    static auto contained_in_fn(const std::set<NFAState *> &set)
    {
        return [&](NFAState *x) {
            return set.contains(x);
        };
    }
    
    void epsilon_close(std::set<NFAState *> &set)
    {
        std::deque<NFAState *> queue { set.cbegin(), set.cend() };

        while (!queue.empty())
        {
            auto state = queue.front();
            queue.pop_front();

            auto iter = state->transitions().find(nullptr);

            if (iter == state->transitions().end())
            {
                continue;
            }

            std::ranges::for_each(
                iter->second | std::ranges::views::filter(std::not_fn(contained_in_fn(set))),
                [&](NFAState *to_state) {
                    set.emplace(to_state);
                    queue.push_back(to_state);
                });
        }
    }

    std::set<NFAState *> transit(const std::set<NFAState *> &states, Symbol *input)
    {
        std::set<NFAState *> to_states;

        for (NFAState *state : states)
        {
            auto iter = state->transitions().find(input);

            if (iter == state->transitions().end())
            {
                continue;
            }

            to_states.insert(iter->second.begin(), iter->second.end());
        }

        epsilon_close(to_states);

        return to_states;
    }
    
    NFA::NFA()
    {
    }

    NFA::NFA(NFA &&nfa)
        : m_start { nfa.m_start },
          m_states { std::move(nfa.m_states) }
    {
        nfa.m_start = nullptr;
    }

    NFA &NFA::operator=(NFA &&nfa)
    {
        m_start = nfa.m_start;
        m_states = std::move(nfa.m_states);

        nfa.m_start = nullptr;
        return *this;
    }
    
    NFAState *NFA::create_state(const Acceptance &acceptance)
    {
        auto ptr = std::make_unique<NFAState>(acceptance);
        auto raw_ptr = ptr.get();

        m_states.push_back(std::move(ptr));

        return raw_ptr;
    }

    using NamedStateCatalog = std::unordered_map<std::pair<Symbol *, Symbol *>, NFAState *, PairHash<Symbol *, Symbol *>>;

    static auto lhs_is_equal_to_fn(Symbol *lhs)
    {
        return [=](Production *p) {
            return p->lhs == lhs;
        };
    }
    
    static void grow_named_state(NFAState *state, Symbol *lhs, Symbol *follow, NFA &nfa, const Grammar &g, NamedStateCatalog &named_states);

    static NFAState *get_named_state(Symbol *lhs, Symbol *follow, NFA &nfa, const Grammar &g, NamedStateCatalog &named_states)
    {
        auto iter = named_states.find(std::pair { lhs, follow });

        if (iter == named_states.end())
        {
            auto state = nfa.create_state(Acceptance { AcceptanceType::reject, nullptr });

            named_states.emplace(std::pair { lhs, follow }, state);

            grow_named_state(state, lhs, follow, nfa, g, named_states);

            return state;
        }
        else
        {
            return iter->second;
        }
    }

    static void grow_named_state_by_production(NFAState *state, Production *p, Symbol *follow, NFA &nfa, const Grammar &g, NamedStateCatalog &named_states)
    {
        std::ranges::subrange rest_rhs { p->rhs };
        
        auto prev_state = state;

        while (!rest_rhs.empty())
        {
            auto curr_input = rest_rhs.front();
            rest_rhs = rest_rhs.next();

            auto curr_state = nfa.create_state(Acceptance { AcceptanceType::reject, nullptr });

            prev_state->add_transition(curr_input, curr_state);

            if (curr_input->is_intermediate())
            {
                auto rest_rhs_first = first(rest_rhs, follow);

                for (Symbol *to_follow : rest_rhs_first)
                {
                    auto to_state = get_named_state(curr_input, to_follow, nfa, g, named_states);

                    prev_state->add_transition(nullptr, to_state);
                }
            }

            prev_state = curr_state;
        }

        auto final_state = nfa.create_state(Acceptance { AcceptanceType::reduce, p });
        
        prev_state->add_transition(follow, final_state);
    }
    
    static void grow_named_state(NFAState *state, Symbol *lhs, Symbol *follow, NFA &nfa, const Grammar &g, NamedStateCatalog &named_states)
    {
        std::ranges::for_each(
            g.productions() | std::ranges::views::filter(lhs_is_equal_to_fn(lhs)),
            [&](Production *p) {
                grow_named_state_by_production(state, p, follow, nfa, g, named_states);
            });
    }
    
    NFA grammar_to_nfa(const Grammar &g)
    {
        NFA nfa;

        NamedStateCatalog named_states;
        
        auto state1 = nfa.create_state(Acceptance { AcceptanceType::reject, nullptr });
        auto state2 = nfa.create_state(Acceptance { AcceptanceType::reject, nullptr });
        auto state3 = nfa.create_state(Acceptance { AcceptanceType::accept, nullptr });
        auto state4 = get_named_state(g.start(), g.end(), nfa, g, named_states);

        state1->add_transition(g.start(), state2);
        state2->add_transition(g.end(), state3);
        state1->add_transition(nullptr, state4);

        nfa.set_start(state1);

        return nfa;
    }
    
}
