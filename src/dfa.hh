#ifndef LR1CC_INCLUDE_DFA_HH
#define LR1CC_INCLUDE_DFA_HH

#include "symbol.hh"
#include "grammar.hh"
#include "nfa.hh"

#include <deque>
#include <map>
#include <memory>
#include <ranges>
#include <type_traits>
#include <unordered_set>
#include <utility>
#include <vector>

namespace lr1cc
{

    class DFAState;

    using DFATransitionCatalog = std::map<Symbol *, DFAState *>;
    
    class DFAState
    {

        bool m_accepts;
        std::set<Production *> m_reductions;
        DFATransitionCatalog m_transitions;
        
    public:

        template <typename R>
        requires std::ranges::input_range<std::remove_reference_t<R>>
        DFAState(R &&);

        DFAState(const DFAState &) = delete;
        DFAState(DFAState &&) = delete;

        DFAState &operator=(const DFAState &) = delete;
        DFAState &operator=(DFAState &&) = delete;

        bool accepts() const;

        const std::set<Production *> &reductions() const;

        bool rejects() const;
        
        const DFATransitionCatalog &transitions() const;
        DFATransitionCatalog &transitions();
        
    };

    template <typename Func>
    void for_each_dfa_state(DFAState *state, Func func);

    template <typename Func>
    void for_each_dfa_state_with_path(DFAState *state, Func func);
    
    class DFA
    {

        DFAState *m_start;
        std::vector<std::unique_ptr<DFAState>> m_states;
        
    public:

        DFA();

        DFA(const DFA &) = delete;
        DFA(DFA &&);

        DFA &operator=(const DFA &) = delete;
        DFA &operator=(DFA &&);
        
        DFAState *start() const;
        void set_start(DFAState *);

        template <typename R>
        requires std::ranges::input_range<std::remove_reference_t<R>>
        DFAState *create_state(R &&);

        template <typename R>
        requires std::ranges::input_range<std::remove_reference_t<R>>
        DFAState *run(R &&) const;
        
    };

    DFA nfa_to_dfa(const NFA &);
    
    template <typename R>
    requires std::ranges::input_range<std::remove_reference_t<R>>
    bool nfa_states_accept(R &&r)
    {
        return std::ranges::any_of(
            r,
            [](NFAState *state) {
                return state->acceptance().type == AcceptanceType::accept;
            });
    }

    template <typename R>
    requires std::ranges::input_range<std::remove_reference_t<R>>
    std::set<Production *> nfa_states_reductions(R &&r)
    {
        std::set<Production *> result;

        auto acceptance_is_reduce = [](NFAState *state) {
            return state->acceptance().type == AcceptanceType::reduce;
        };
        
        std::ranges::for_each(
            r | std::ranges::views::filter(acceptance_is_reduce),
            [&](NFAState *state) {
                result.emplace(state->acceptance().production);
            });

        return result;
    }
    
    template <typename R>
    requires std::ranges::input_range<std::remove_reference_t<R>>
    DFAState::DFAState(R &&r)
        : m_accepts { nfa_states_accept(r) },
          m_reductions { nfa_states_reductions(r) }
    {
    }

    inline bool DFAState::accepts() const
    {
        return m_accepts;
    }

    inline const std::set<Production *> &DFAState::reductions() const
    {
        return m_reductions;
    }

    inline bool DFAState::rejects() const
    {
        return !m_accepts && m_reductions.empty();
    }
    
    inline const DFATransitionCatalog &DFAState::transitions() const
    {
        return m_transitions;
    }

    inline DFATransitionCatalog &DFAState::transitions()
    {
        return m_transitions;
    }

    inline DFAState *DFA::start() const
    {
        return m_start;
    }

    inline void DFA::set_start(DFAState *s)
    {
        m_start = s;
    }

    template <typename R>
    requires std::ranges::input_range<std::remove_reference_t<R>>
    DFAState *DFA::create_state(R &&r)
    {
        auto ptr = std::make_unique<DFAState>(r);
        auto raw_ptr = ptr.get();

        m_states.push_back(std::move(ptr));

        return raw_ptr;
    }

    template <typename R>
    requires std::ranges::input_range<std::remove_reference_t<R>>
    DFAState *DFA::run(R &&r) const
    {
        auto state = m_start;

        for (Symbol *input : r)
        {
            auto iter = state->transitions().find(input);

            if (iter == state->transitions().end())
            {
                state = nullptr;
                break;
            }
            else
            {
                state = iter->second;
            }
        }

        return state;
    }

    template <typename Func>
    void for_each_dfa_state(DFAState *state, Func func)
    {
        std::unordered_set<DFAState *> visited { state };
        std::deque<DFAState *> queue;
        queue.push_back(state);

        while (!queue.empty())
        {
            auto s = queue.front();
            queue.pop_front();

            func(s);

            for (const auto &pair : s->transitions())
            {
                if (!visited.contains(pair.second))
                {
                    visited.emplace(pair.second);
                    queue.push_back(pair.second);
                }
            }
        }
    }

    template <typename Func>
    void for_each_dfa_state_with_path(DFAState *state, Func func)
    {
        std::unordered_set<DFAState *> visited { state };
        std::deque<std::pair<DFAState *, std::vector<Symbol *>>> queue;
        queue.push_back(std::pair { state, std::vector<Symbol *> { } });

        while (!queue.empty())
        {
            auto [ state, path ] = queue.front();
            queue.pop_front();

            func(state, path);

            for (const auto &pair : state->transitions())
            {
                if (!visited.contains(pair.second))
                {
                    visited.emplace(pair.second);
                    
                    std::vector new_path { path };
                    new_path.push_back(pair.first);
                    
                    queue.push_back(std::pair { pair.second, new_path });
                }
            }
        }
    }
    
}

#endif
