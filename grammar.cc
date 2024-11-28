
#include "grammar.hh"

namespace lr1cc
{

    Grammar::Grammar()
        : m_start { nullptr },
          m_end { nullptr }
    {
    }

    void Grammar::calculate() const
    {
        calculate_nullable();
        calculate_first();
    }

    static bool lhs_nullable_should_updated(Production *p)
    {
        return !p->lhs->is_nullable()
            && is_nullable(p->rhs);
    }
    
    void Grammar::calculate_nullable() const
    {
        bool updated = true;

        while (updated)
        {
            updated = false;

            std::ranges::for_each(
                m_productions | std::ranges::views::filter(lhs_nullable_should_updated),
                [&](Production *p) {
                    updated = true;
                    p->lhs->set_nullable();
                });
        }
    }

    void Grammar::calculate_first() const
    {
        bool updated = true;

        while (updated)
        {
            updated = false;

            for (Production *p : m_productions)
            {
                auto prev_size = p->lhs->first().size();

                auto rhs_first = first(p->rhs);
                p->lhs->first().insert(rhs_first.cbegin(), rhs_first.cend());

                updated = updated || prev_size != p->lhs->first().size();
            }
        }
    }
    
}
