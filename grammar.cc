
#include "grammar.hh"

#include <sstream>
#include <stdexcept>
#include <utility>

namespace lr1cc
{

    Grammar::Grammar()
        : m_start { nullptr },
          m_end { nullptr }
    {
    }

    Grammar::Grammar(Grammar &&g)
        : m_start { g.m_start },
          m_end { g.m_end },
          m_productions { std::move(g.m_productions) }
    {
        g.m_start = nullptr;
        g.m_end = nullptr;
    }

    Grammar &Grammar::operator=(Grammar &&g)
    {
        m_start = g.m_start;
        m_end = g.m_end;
        m_productions = std::move(g.m_productions);

        g.m_start = nullptr;
        g.m_end = nullptr;

        return *this;
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
    
    void Grammar::ensure_production_sanity(Production *p) const
    {
        if (!p->lhs->is_intermediate())
        {
            std::ostringstream msg;

            msg << "error: invalid lhs symbol `"
                << p->lhs->name()
                << "' on ["
                << p->name
                << "].\n";
            
            throw std::runtime_error { msg.str() };
        }

        for (Symbol *s : p->rhs)
        {
            if (s == m_end)
            {
                std::ostringstream msg;

                msg << "error: invalid rhs symbol `"
                    << s->name()
                    << "' on ["
                    << p->name
                    << "].\n";

                throw std::runtime_error { msg.str() };
            }
        }
    }
    
    void Grammar::ensure_sanity() const
    {
        if (m_start == nullptr)
        {
            throw std::runtime_error { "error: start symbol is not provided.\n" };
        }

        if (m_end == nullptr)
        {
            throw std::runtime_error { "error: End-Of-Tokens symbol is not provided.\n" };
        }

        for (Production *p : m_productions)
        {
            ensure_production_sanity(p);
        }
    }
    
}
