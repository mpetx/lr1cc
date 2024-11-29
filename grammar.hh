#ifndef LR1CC_INCLUDE_GRAMMAR_HH
#define LR1CC_INCLUDE_GRAMMAR_HH

#include "symbol.hh"

#include <vector>

namespace lr1cc
{

    struct Production
    {
        std::string name;
        Symbol *lhs;
        std::vector<Symbol *> rhs;
    };

    using ProductionCatalog = std::vector<Production *>;
    
    class Grammar
    {

        Symbol *m_start;
        Symbol *m_end;
        ProductionCatalog m_productions;

        void calculate_nullable() const;
        void calculate_first() const;
        
    public:

        Grammar();

        Grammar(const Grammar &) = delete;
        Grammar(Grammar &&);

        Grammar &operator=(const Grammar &) = delete;
        Grammar &operator=(Grammar &&);

        Symbol *start() const;
        void set_start(Symbol *);

        Symbol *end() const;
        void set_end(Symbol *);

        const ProductionCatalog &productions() const;
        ProductionCatalog &productions();

        void calculate() const;
        
    };

    inline Symbol *Grammar::start() const
    {
        return m_start;
    }

    inline void Grammar::set_start(Symbol *s)
    {
        m_start = s;
    }

    inline Symbol *Grammar::end() const
    {
        return m_end;
    }

    inline void Grammar::set_end(Symbol *s)
    {
        m_end = s;
    }

    inline const ProductionCatalog &Grammar::productions() const
    {
        return m_productions;
    }
    
    inline ProductionCatalog &Grammar::productions()
    {
        return m_productions;
    }
    
}

#endif
