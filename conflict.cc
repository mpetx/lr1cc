
#include "conflict.hh"

#include <algorithm>
#include <deque>
#include <functional>
#include <ranges>
#include <unordered_set>

namespace lr1cc
{

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

    static bool has_reduce_reduce_conflict(DFAState *state)
    {
        return state->accepts() && !state->reductions().empty()
            || state->reductions().size() > 1;
    }

    static bool has_shift_reduce_conflict(DFAState *state)
    {
        return (state->accepts() || !state->reductions().empty())
            && !state->transitions().empty();
    }
    
    static void collect_conflicts_of_first(DFAState *first, const std::vector<Symbol *> &first_path, std::vector<Conflict> &conflicts)
    {
        if (has_reduce_reduce_conflict(first))
        {
            conflicts.push_back(Conflict { first, first, first_path, std::vector<Symbol *> { } });
        }

        if (has_shift_reduce_conflict(first))
        {
            for_each_dfa_state_with_path(
                first,
                [&](DFAState *second, const std::vector<Symbol *> &second_path) {
                    if (!second->rejects() && first != second)
                    {
                        conflicts.push_back(Conflict { first, second, first_path, second_path });
                    }
                });
        }
    }
    
    std::vector<Conflict> collect_conflicts(const DFA &dfa)
    {
        std::vector<Conflict> conflicts;

        for_each_dfa_state_with_path(
            dfa.start(),
            [&](DFAState *state, const std::vector<Symbol *> &path) {
                collect_conflicts_of_first(state, path, conflicts);
            });

        return conflicts;
    }
    
}
