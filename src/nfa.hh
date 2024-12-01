#ifndef LR1CC_INCLUDE_NFA_HH
#define LR1CC_INCLUDE_NFA_HH

#include "symbol.hh"
#include "grammar.hh"

#include <map>
#include <memory>
#include <ranges>
#include <set>
#include <type_traits>
#include <vector>

namespace lr1cc
{

    enum class AcceptanceType
    {
        reject, accept, reduce
    };

    struct Acceptance
    {
        AcceptanceType type;
        Production *production;
    };

    class NFAState;

    using NFATransitionCatalog = std::map<Symbol *, std::set<NFAState *>>;
    
    class NFAState
    {

        Acceptance m_acceptance;
        NFATransitionCatalog m_transitions;

    public:

        NFAState(const Acceptance &);
        
        NFAState(const NFAState &) = delete;
        NFAState(NFAState &&) = delete;

        NFAState &operator=(const NFAState &) = delete;
        NFAState &operator=(NFAState &&) = delete;

        const Acceptance &acceptance() const;

        const NFATransitionCatalog &transitions() const;
        NFATransitionCatalog &transitions();

        void add_transition(Symbol *, NFAState *);
        
    };

    void epsilon_close(std::set<NFAState *> &);
    std::set<NFAState *> transit(const std::set<NFAState *> &, Symbol *);
    
    class NFA
    {

        NFAState *m_start;
        std::vector<std::unique_ptr<NFAState>> m_states;

    public:

        NFA();
        
        NFA(const NFA &) = delete;
        NFA(NFA &&);

        NFA &operator=(const NFA &) = delete;
        NFA &operator=(NFA &&);

        NFAState *start() const;
        void set_start(NFAState *);
        
        NFAState *create_state(const Acceptance &);

        template <typename R>
        requires std::ranges::input_range<std::remove_reference_t<R>>
        std::set<NFAState *> run(R &&inputs) const;
        
    };

    NFA grammar_to_nfa(const Grammar &);
    
    inline const Acceptance &NFAState::acceptance() const
    {
        return m_acceptance;
    }

    inline const NFATransitionCatalog &NFAState::transitions() const
    {
        return m_transitions;
    }
    
    inline NFATransitionCatalog &NFAState::transitions()
    {
        return m_transitions;
    }
    
    inline NFAState *NFA::start() const
    {
        return m_start;
    }
    
    inline void NFA::set_start(NFAState *s)
    {
        m_start = s;
    }

    template <typename R>
    requires std::ranges::input_range<std::remove_reference_t<R>>
    std::set<NFAState *> NFA::run(R &&inputs) const
    {
        std::set states { m_start };
        epsilon_close(states);

        for (Symbol *input : inputs)
        {
            states = transit(states, input);
        }

        return states;
    }
    
}

#endif
