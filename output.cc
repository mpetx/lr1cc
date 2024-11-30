
#include "output.hh"

#include <deque>
#include <unordered_map>
#include <unordered_set>

namespace lr1cc
{

    static std::unordered_map<DFAState *, std::size_t> name_states(const DFA &dfa)
    {
        std::unordered_map<DFAState *, std::size_t> state_to_name;

        for_each_dfa_state(
            dfa.start(),
            [&](DFAState *state) {
                if (state->rejects())
                {
                    std::size_t name = state_to_name.size() + 1;
                    state_to_name.emplace(state, name);
                }
            });

        return state_to_name;
    }

    void output_lr1_table_header(const std::vector<Symbol *> &columns, std::ostream &out)
    {
        for (const auto &column : columns)
        {
            out << ',' << column->name();
        }

        out << "\r\n";
    }

    void output_lr1_table_cell(DFAState *state, Symbol *column, std::unordered_map<DFAState *, std::size_t> &state_to_name, std::ostream &out)
    {
        auto iter = state->transitions().find(column);

        if (iter == state->transitions().end())
        {
            return;
        }

        auto to_state = iter->second;

        if (to_state->accepts())
        {
            out << 'A';
        }
        else if (!to_state->reductions().empty())
        {
            out << 'R' << (*to_state->reductions().begin())->name;
        }
        else
        {
            out << (column->is_terminal() ? 'S' : 'G') << state_to_name.at(to_state);
        }
    }
    
    void output_lr1_table_row(DFAState *state, const std::vector<Symbol *> &columns, std::unordered_map<DFAState *, std::size_t> &state_to_name, std::ostream &out)
    {
        out << state_to_name.at(state);

        for (Symbol *column : columns)
        {
            out << ',';

            output_lr1_table_cell(state, column, state_to_name, out);
        }

        out << "\r\n";
    }
    
    void output_lr1_table(const DFA &dfa, const std::vector<Symbol *> &columns, std::ostream &out)
    {
        auto state_to_name = name_states(dfa);
        
        output_lr1_table_header(columns, out);

        for_each_dfa_state(
            dfa.start(),
            [&](DFAState *state) {
                if (state->rejects())
                {
                    output_lr1_table_row(state, columns, state_to_name, out);
                }
            });

        out << std::flush;
    }
    
}
