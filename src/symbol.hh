#ifndef LR1CC_INCLUDE_SYMBOL_HH
#define LR1CC_INCLUDE_SYMBOL_HH

#include "util.hh"

#include <algorithm>
#include <functional>
#include <memory>
#include <ranges>
#include <set>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <vector>

namespace lr1cc
{

    enum class SymbolType : char
    {
        terminal, intermediate
    };

    class Symbol;

    using First = std::set<Symbol *>;
    
    class Symbol
    {

        std::string m_name;
        SymbolType m_type;
        bool m_nullable;
        First m_first;

    public:

        Symbol(std::string_view, SymbolType);

        Symbol(const Symbol &) = delete;
        Symbol(Symbol &&) = delete;

        Symbol &operator=(const Symbol &) = delete;
        Symbol &operator=(Symbol &&) = delete;
        
        const std::string &name() const;

        SymbolType type() const;
        bool is_terminal() const;
        bool is_intermediate() const;

        bool is_nullable() const;
        void set_nullable();

        const First &first() const;
        First &first();
        
    };

    template <typename R>
    requires std::ranges::input_range<std::remove_reference_t<R>>
    bool is_nullable(R &&r);

    template <typename R>
    requires std::ranges::input_range<std::remove_reference_t<R>>
    std::set<Symbol *> first(R &&, Symbol * = nullptr);
        
    class SymbolManager
    {

        std::vector<std::unique_ptr<Symbol>> m_symbols;
        std::unordered_map<std::string, Symbol *, HeterogeneousStringHash, std::equal_to<>> m_name_to_symbol;

    public:

        SymbolManager();
        
        SymbolManager(const SymbolManager &) = delete;
        SymbolManager(SymbolManager &&) = delete;

        SymbolManager &operator=(const SymbolManager &) = delete;
        SymbolManager &operator=(SymbolManager &&) = delete;

        Symbol *create_symbol(std::string_view, SymbolType);

        Symbol *get_symbol(std::string_view) const;

        auto symbols() const;
        
    };

    inline const std::string &Symbol::name() const
    {
        return m_name;
    }

    inline SymbolType Symbol::type() const
    {
        return m_type;
    }

    inline bool Symbol::is_terminal() const
    {
        return m_type == SymbolType::terminal;
    }

    inline bool Symbol::is_intermediate() const
    {
        return m_type == SymbolType::intermediate;
    }

    inline bool Symbol::is_nullable() const
    {
        return m_nullable;
    }

    inline void Symbol::set_nullable()
    {
        m_nullable = true;
    }
    
    inline const First &Symbol::first() const
    {
        return m_first;
    }

    inline First &Symbol::first()
    {
        return m_first;
    }

    template <typename R>
    requires std::ranges::input_range<std::remove_reference_t<R>>
    bool is_nullable(R &&r)
    {
        return std::ranges::all_of(r, [](Symbol *s) {
            return s->is_nullable();
        });
    }

    template <typename R>
    requires std::ranges::input_range<std::remove_reference_t<R>>
    std::set<Symbol *> first(R &&r, Symbol *sentinel)
    {
        std::set<Symbol *> result;

        for (Symbol *s : r)
        {
            result.insert(s->first().cbegin(), s->first().cend());

            if (!s->is_nullable())
            {
                return result;
            }
        }

        if (sentinel != nullptr)
        {
            result.emplace(sentinel);
        }

        return result;
    }

    inline auto SymbolManager::symbols() const
    {
        auto raw_pointer = [](const std::unique_ptr<Symbol> &p) {
            return p.get();
        };
        
        return m_symbols | std::ranges::views::transform(raw_pointer);
    }
    
}

#endif
