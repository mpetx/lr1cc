#ifndef LR1CC_INCLUDE_INPUT_PARSER_HH
#define LR1CC_INCLUDE_INPUT_PARSER_HH

#include "grammar.hh"
#include "input-lexer.hh"

#include <iostream>
#include <memory>
#include <vector>

namespace lr1cc
{

    Grammar parse_input(std::istream &in, SymbolManager &manager, std::vector<std::unique_ptr<Production>> &productions);
    
}

#endif
