
#include "symbol.hh"

#include <stdexcept>
#include <utility>

namespace lr1cc
{

    Symbol::Symbol(std::string_view name, SymbolType type)
        : m_name { name },
          m_type { type },
          m_nullable { false }
    {
        if (m_type == SymbolType::terminal)
        {
            m_first.emplace(this);
        }
    }

    SymbolManager::SymbolManager()
    {
    }

    Symbol *SymbolManager::create_symbol(std::string_view name, SymbolType type)
    {
        if (m_name_to_symbol.contains(name))
        {
            return nullptr;
        }
        
        auto ptr = std::make_unique<Symbol>(name, type);
        auto raw_ptr = ptr.get();

        m_symbols.push_back(std::move(ptr));

        m_name_to_symbol.emplace(name, raw_ptr);

        return raw_ptr;
    }

    Symbol *SymbolManager::get_symbol(std::string_view name) const
    {
        auto iter = m_name_to_symbol.find(name);

        if (iter == m_name_to_symbol.end())
        {
            return nullptr;
        }
        else
        {
            return iter->second;
        }
    }
    
}
