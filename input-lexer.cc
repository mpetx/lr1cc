
#include "input.hh"

#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace lr1cc
{

    Lexer::Lexer(std::istream &in)
        : m_in { in },
          m_line { 1 }
    {
    }
    
    int Lexer::front_char()
    {
        if (m_front_char.has_value())
        {
            return m_front_char.value();
        }

        m_front_char = m_in.get();
        
        return m_front_char.value();
    }

    void Lexer::pop_char()
    {
        if (m_front_char == '\n')
        {
            ++m_line;
        }
        
        m_front_char = std::nullopt;
    }

    void Lexer::skip_spaces()
    {
        while (true)
        {
            int ch = front_char();

            if (ch == -1)
            {
                break;
            }
            else if (std::isspace(ch))
            {
                pop_char();
            }
            else if (ch == '#')
            {
                pop_char();
                skip_line();
            }
            else
            {
                break;
            }
        }
    }

    void Lexer::skip_line()
    {
        while (true)
        {
            int ch = front_char();

            if (ch == -1)
            {
                break;
            }
            else if (ch == '\n')
            {
                pop_char();
                break;
            }
            else
            {
                pop_char();
            }
        }
    }

    static bool is_ident_char(int ch)
    {
        return std::isalnum(ch)
            || ch == '_'
            || ch == '-'
            || ch == '.';
    }

    std::string Lexer::read_ident_string()
    {
        std::string str;

        while (true)
        {
            int ch = front_char();

            if (ch == -1)
            {
                break;
            }
            else if (is_ident_char(ch))
            {
                pop_char();
                str.push_back(ch);
            }
            else
            {
                break;
            }
        }

        return str;
    }

    void Lexer::load_section_marker()
    {
        std::string name = read_ident_string();

        if (name == "start")
        {
            m_front = Token { TokenType::start_marker, "%start" };
        }
        else if (name == "end")
        {
            m_front = Token { TokenType::end_marker, "%end" };
        }
        else if (name == "terminal")
        {
            m_front = Token { TokenType::terminal_marker, "%terminal" };
        }
        else if (name == "intermediate")
        {
            m_front = Token { TokenType::intermediate_marker, "%intermediate" };
        }
        else if (name == "grammar")
        {
            m_front = Token { TokenType::grammar_marker, "%grammar" };
        }
        else
        {
            std::ostringstream msg;
            msg << "lexical error: unknown section marker `%"
                << name
                << "' at line "
                << line()
                << ".\n";
            throw std::runtime_error { msg.str() };
        }
    }

    void Lexer::load_ident()
    {
        std::string value = read_ident_string();
        m_front = Token { TokenType::ident, value };
    }
    
    Token Lexer::front()
    {
        if (m_front.has_value())
        {
            return m_front.value();
        }

        skip_spaces();

        int ch1 = front_char();

        if (ch1 == -1)
        {
            m_front = Token { TokenType::end, "EOF" };
        }
        else if (ch1 == '%')
        {
            pop_char();
            load_section_marker();
        }
        else if (ch1 == ':')
        {
            pop_char();
            m_front = Token { TokenType::colon, ":" };
        }
        else if (ch1 == '|')
        {
            pop_char();
            m_front = Token { TokenType::bar, "|" };
        }
        else if (ch1 == ';')
        {
            pop_char();
            m_front = Token { TokenType::semicolon, ";" };
        }
        else if (ch1 == '[')
        {
            pop_char();
            m_front = Token { TokenType::square_start, "[" };
        }
        else if (ch1 == ']')
        {
            pop_char();
            m_front = Token { TokenType::square_end, "]" };
        }
        else if (is_ident_char(ch1))
        {
            load_ident();
        }
        else
        {
            std::ostringstream msg;
            msg << "lexical error: unexpected character `"
                << static_cast<char>(ch1)
                << "' (0x"
                << std::setw(2) << std::setfill('0') << std::hex
                << ch1
                << ") at line "
                << std::setfill(' ') << std::dec
                << line()
                << ".\n";
            throw std::runtime_error { msg.str() };
        }

        return m_front.value();
    }
}
