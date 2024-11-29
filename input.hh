#ifndef LR1CC_INCLUDE_INPUT_HH
#define LR1CC_INCLUDE_INPUT_HH

#include "grammar.hh"

#include <iostream>
#include <optional>
#include <memory>
#include <string>
#include <vector>

namespace lr1cc
{

    enum class TokenType
    {
        ident,
        start_marker,
        end_marker,
        terminal_marker,
        intermediate_marker,
        grammar_marker,
        colon,
        bar,
        semicolon,
        square_start,
        square_end,
        end
    };

    struct Token
    {
        TokenType type;
        std::string value;
    };
    
    class Lexer
    {

        std::istream &m_in;
        std::optional<int> m_front_char;
        std::size_t m_line;
        std::optional<Token> m_front;
        
        int front_char();
        void pop_char();

        void skip_line();
        void skip_spaces();

        std::string read_ident_string();
        
        void load_section_marker();
        void load_ident();
        
    public:

        Lexer(std::istream &);

        Lexer(const Lexer &) = delete;
        Lexer(Lexer &&) = delete;

        Lexer &operator=(const Lexer &) = delete;
        Lexer &operator=(Lexer &&) = delete;

        std::size_t line() const;

        Token front();
        void pop();
        
    };

    Grammar parse_input(std::istream &, SymbolManager &, std::vector<std::unique_ptr<Production>> &);
    
    inline std::size_t Lexer::line() const
    {
        return m_line;
    }

    inline void Lexer::pop()
    {
        m_front = std::nullopt;
    }
    
}

#endif
