
#include "cli.hh"
#include "grammar.hh"
#include "nfa.hh"
#include "dfa.hh"
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
        auto nfa = lr1cc::grammar_to_nfa(g);
        auto dfa = lr1cc::nfa_to_dfa(nfa);

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
