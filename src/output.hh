#ifndef LR1CC_INCLUDE_OUTPUT_HH
#define LR1CC_INCLUDE_OUTPUT_HH

#include "dfa.hh"

#include <iostream>
#include <vector>

namespace lr1cc
{

    void output_lr1_table(const DFA &, const std::vector<Symbol *> &, std::ostream &);
    
}

#endif
