
#include "cli.hh"
#include "grammar.hh"
#include "nfa.hh"
#include "dfa.hh"
#include "conflict.hh"
#include "input.hh"
#include "output.hh"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>

static void print_help()
{
    std::cerr << "usage: lr1cc [-o outfile] [-h] infile" << std::endl;
}

static void output_conflict_point(const std::vector<lr1cc::Symbol *> &symbols, std::string_view point)
{
    for (std::size_t i = 0; i < symbols.size() - 1; ++i)
    {
        std::cerr << symbols[i]->name() << ' ';
    }

    std::cerr << point << ' ';

    std::cerr << symbols[symbols.size() - 1]->name();
}

static void output_actions(lr1cc::DFAState *state)
{
    if (state->accepts())
    {
        std::cerr << " *ACCEPT*";
    }

    for (lr1cc::Production *p : state->reductions())
    {
        std::cerr << ' ' << p->name;
    }
}

static void report_sr_conflict(const lr1cc::Conflict &conflict)
{
    output_conflict_point(conflict.start_to_first, "[1]");

    std::cerr << ' ';
        
    output_conflict_point(conflict.first_to_second, "[2]");

    std::cerr << '\n';

    std::cerr << "[1]:";
    output_actions(conflict.first_state);
    
    std::cerr << "\n[2]:";
    output_actions(conflict.second_state);

    std::cerr << "\n\n";
}

static void report_rr_conflict(const lr1cc::Conflict &conflict)
{
    output_conflict_point(conflict.start_to_first, "[1]");

    std::cerr << '\n';

    std::cerr << "[1]:";
    output_actions(conflict.first_state);

    std::cerr << "\n\n";
}

static void report_conflict(const lr1cc::Conflict &conflict)
{
    if (conflict.first_state == conflict.second_state)
    {
        report_rr_conflict(conflict);
    }
    else
    {
        report_sr_conflict(conflict);
    }
}

static void report_conflicts(const std::vector<lr1cc::Conflict> &conflicts)
{
    for (const lr1cc::Conflict &conflict : conflicts)
    {
        report_conflict(conflict);
    }
}

static std::vector<lr1cc::Symbol *> calculate_columns(const lr1cc::SymbolManager &manager)
{
    std::vector<lr1cc::Symbol *> columns;

    auto push_column = [&](lr1cc::Symbol *s) {
        columns.push_back(s);
    };
    
    auto symbol_is_terminal = [](lr1cc::Symbol *s) {
        return s->is_terminal();
    };

    auto symbol_is_intermediate = [](lr1cc::Symbol *s) {
        return s->is_intermediate();
    };
    
    std::ranges::for_each(
        manager.symbols() | std::ranges::views::filter(symbol_is_terminal),
        push_column);

    std::ranges::for_each(
        manager.symbols() | std::ranges::views::filter(symbol_is_intermediate),
        push_column);
    
    return columns;
}

int main(int argc_, char **argv_)
{
    std::vector<std::string> argv { argv_, argv_ + argc_ };
    
    auto conf = lr1cc::parse_argv(argv);

    if (!conf.has_value())
    {
        std::cerr << "error: invalid command line arguments." << std::endl;
        print_help();
        return 1;
    }

    if (conf.value().help)
    {
        print_help();
        return 0;
    }
    
    lr1cc::SymbolManager manager;
    std::vector<std::unique_ptr<lr1cc::Production>> productions;
    
    try
    {
        std::ifstream in { conf.value().input_file };
        std::ofstream out { conf.value().output_file, std::ios::binary };

        if (!in)
        {
            std::cerr << "error: failed to open `" << conf.value().input_file << "'." << std::endl;
            return 1;
        }

        if (!out)
        {
            std::cerr << "error: failed to open `" << conf.value().output_file << "'." << std::endl;;
            return 1;
        }
        
        auto g = lr1cc::parse_input(in, manager, productions);
        g.calculate();
        g.ensure_sanity();
        
        auto nfa = lr1cc::grammar_to_nfa(g);
        auto dfa = lr1cc::nfa_to_dfa(nfa);

        auto conflicts = collect_conflicts(dfa);

        if (!conflicts.empty())
        {
            std::cerr << conflicts.size()
                      << (conflicts.size() == 1 ? " conflict" : " conflicts")
                      << " detected.\n";
            
            report_conflicts(conflicts);
            
            std::cerr << std::flush;
            return 1;
        }
        
        auto columns = calculate_columns(manager);

        lr1cc::output_lr1_table(dfa, columns, out);
    }
    catch (std::ios_base::failure &e)
    {
        std::cerr << "error: I/O failed." << std::endl;
        return 1;
    }
    catch (std::runtime_error &e)
    {
        std::cerr << e.what() << std::flush;
        return 1;
    }
    
    return 0;
}
