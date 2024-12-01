#ifndef LR1CC_INCLUDE_CONFLICT_HH
#define LR1CC_INCLUDE_CONFLICT_HH

#include "dfa.hh"

namespace lr1cc
{

    struct Conflict
    {
        DFAState *first_state;
        DFAState *second_state;
        std::vector<Symbol *> start_to_first;
        std::vector<Symbol *> first_to_second;
    };

    std::vector<Conflict> collect_conflicts(const DFA &);
    
}

#endif
