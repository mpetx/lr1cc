
#include "nfa.hh"

#include <algorithm>
#include <deque>
#include <ranges>
#include <utility>

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
    
    NFAState *NFA::create_state(const Acceptance &acceptance)
    {
        auto ptr = std::make_unique<NFAState>(acceptance);
        auto raw_ptr = ptr.get();

        m_states.push_back(std::move(ptr));

        return raw_ptr;
    }
    
}
