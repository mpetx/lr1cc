#ifndef LR1CC_INCLUDE_SYMBOL_HH
#define LR1CC_INCLUDE_SYMBOL_HH

#include <functional>
#include <memory>
#include <set>
#include <string>
#include <string_view>
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

    struct StringHash
    {
        using is_transparent = void;
        
        std::size_t operator()(std::string_view) const;
        std::size_t operator()(const std::string &) const;
    };
        
    class SymbolManager
    {

        std::vector<std::unique_ptr<Symbol>> symbols;
        std::unordered_map<std::string, Symbol *, StringHash, std::equal_to<>> name_to_symbol;

    public:

        SymbolManager();
        
        SymbolManager(const SymbolManager &) = delete;
        SymbolManager(SymbolManager &&) = delete;

        SymbolManager &operator=(const SymbolManager &) = delete;
        SymbolManager &operator=(SymbolManager &&) = delete;

        Symbol *create_symbol(std::string_view, SymbolType);

        Symbol *get_symbol(std::string_view) const;
        
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

    inline std::size_t StringHash::operator()(std::string_view sv) const
    {
        return std::hash<std::string_view>{}(sv);
    }

    inline std::size_t StringHash::operator()(const std::string &s) const
    {
        return std::hash<std::string_view>{}(s);
    }
    
}

#endif
