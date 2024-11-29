
#include "input.hh"

#include <sstream>
#include <stdexcept>
#include <string_view>

namespace lr1cc
{

    static void parse_input_language(Lexer &, Grammar &, SymbolManager &, std::vector<std::unique_ptr<Production>> &);
    static void parse_start_section(Lexer &, Grammar &, SymbolManager &);
    static void parse_end_section(Lexer &, Grammar &, SymbolManager &);
    static void parse_terminal_section(Lexer &, Grammar &, SymbolManager &);
    static void parse_intermediate_section(Lexer &, Grammar &, SymbolManager &);
    static void parse_grammar_section(Lexer &, Grammar &, SymbolManager &, std::vector<std::unique_ptr<Production>> &);
    static void parse_production(Lexer &, Grammar &, SymbolManager &, std::vector<std::unique_ptr<Production>> &);
    static void parse_rhs_1(Lexer &, Symbol *, Grammar &, SymbolManager &, std::vector<std::unique_ptr<Production>> &);

    [[noreturn]] static void unexpected_token(const Token &token, Lexer &lexer, std::string_view expected)
    {
        std::ostringstream msg;

        msg << "syntax error: unexpected token `"
            << token.value
            << "' at line "
            << lexer.line()
            << ", expecting "
            << expected
            << ".\n";

        throw std::runtime_error { msg.str() };
    }

    static void consume_token(Lexer &lexer, TokenType type, std::string_view type_name)
    {
        auto tok = lexer.front();

        if (tok.type != type)
        {
            unexpected_token(tok, lexer, type_name);
        }

        lexer.pop();
    }
    
    Grammar parse_input(std::istream &in, SymbolManager &manager, std::vector<std::unique_ptr<Production>> &productions)
    {
        Grammar g;
        Lexer lexer { in };

        parse_input_language(lexer, g, manager, productions);

        return g;
    }

    static void parse_input_language(Lexer &lexer, Grammar &g, SymbolManager &manager, std::vector<std::unique_ptr<Production>> &productions)
    {
        while (true)
        {
            auto tok = lexer.front();

            if (tok.type == TokenType::start_marker)
            {
                lexer.pop();
                parse_start_section(lexer, g, manager);
            }
            else if (tok.type == TokenType::end_marker)
            {
                lexer.pop();
                parse_end_section(lexer, g, manager);
            }
            else if (tok.type == TokenType::terminal_marker)
            {
                lexer.pop();
                parse_terminal_section(lexer, g, manager);
            }
            else if (tok.type == TokenType::intermediate_marker)
            {
                lexer.pop();
                parse_intermediate_section(lexer, g, manager);
            }
            else if (tok.type == TokenType::grammar_marker)
            {
                lexer.pop();
                parse_grammar_section(lexer, g, manager, productions);
            }
            else if (tok.type == TokenType::end)
            {
                break;
            }
            else
            {
                unexpected_token(tok, lexer, "section marker or EOF");
            }
        }
        
    }

    static void parse_start_section(Lexer &lexer, Grammar &g, SymbolManager &manager)
    {
        if (g.start() != nullptr)
        {
            std::ostringstream msg;
            msg << "error: start symbol redeclared at line " << lexer.line() << ".\n";
            throw std::runtime_error { msg.str() };
        }
        
        auto tok = lexer.front();

        if (tok.type != TokenType::ident)
        {
            unexpected_token(tok, lexer, "an identifier");
        }

        auto s = manager.create_symbol(tok.value, SymbolType::intermediate);

        if (s == nullptr)
        {
            std::ostringstream msg;
            msg << "error: symbol `" << tok.value << "' redeclared at line " << lexer.line() << ".\n";
            throw std::runtime_error { msg.str() };
        }

        g.set_start(s);
        lexer.pop();
    }
    
    static void parse_end_section(Lexer &lexer, Grammar &g, SymbolManager &manager)
    {
        if (g.end() != nullptr)
        {
            std::ostringstream msg;
            msg << "error: End-Of-Tokens symbol redeclared at line " << lexer.line() << ".\n";
            throw std::runtime_error { msg.str() };
        }
        
        auto tok = lexer.front();

        if (tok.type != TokenType::ident)
        {
            std::ostringstream msg;
            msg << "syntax error: unexpected token `"
                << tok.value
                << "' at line "
                << lexer.line()
                << ", expecting an identifier.\n";

            throw std::runtime_error { msg.str() };
        }

        auto s = manager.create_symbol(tok.value, SymbolType::terminal);

        if (s == nullptr)
        {
            std::ostringstream msg;
            msg << "error: symbol `" << tok.value << "' redeclared at line " << lexer.line() << ".\n";
            throw std::runtime_error { msg.str() };
        }

        g.set_end(s);
        lexer.pop();
    }
    
    static void parse_terminal_section(Lexer &lexer, Grammar &g, SymbolManager &manager)
    {
        while (true)
        {
            auto tok = lexer.front();

            if (tok.type != TokenType::ident)
            {
                break;
            }

            auto s = manager.create_symbol(tok.value, SymbolType::terminal);

            if (s == nullptr)
            {
                std::ostringstream msg;
                msg << "error: symbol `" << tok.value << "' redeclared at line " << lexer.line() << ".\n";
                throw std::runtime_error { msg.str() };
            }

            lexer.pop();
        }
    }
    
    static void parse_intermediate_section(Lexer &lexer, Grammar &g, SymbolManager &manager)
    {
        while (true)
        {
            auto tok = lexer.front();

            if (tok.type != TokenType::ident)
            {
                break;
            }

            auto s = manager.create_symbol(tok.value, SymbolType::intermediate);

            if (s == nullptr)
            {
                std::ostringstream msg;
                msg << "error: symbol `" << tok.value << "' redeclared at line " << lexer.line() << ".\n";
                throw std::runtime_error { msg.str() };
            }

            lexer.pop();
        }
    }
    
    static void parse_grammar_section(Lexer &lexer, Grammar &g, SymbolManager &manager, std::vector<std::unique_ptr<Production>> &productions)
    {
        while (true)
        {
            auto tok = lexer.front();

            if (tok.type != TokenType::ident)
            {
                break;
            }

            parse_production(lexer, g, manager, productions);
        }
    }

    static void parse_production(Lexer &lexer, Grammar &g, SymbolManager &manager, std::vector<std::unique_ptr<Production>> &productions)
    {
        auto lhs_tok = lexer.front();

        auto lhs = manager.get_symbol(lhs_tok.value);

        if (lhs == nullptr)
        {
            std::ostringstream msg;
            msg << "error: unknown symbol `" << lhs_tok.value << "' at line " << lexer.line() << ".\n";
            throw std::runtime_error { msg.str() };
        }

        lexer.pop();
        consume_token(lexer, TokenType::colon, "`:'");

        parse_rhs_1(lexer, lhs, g, manager, productions);

        while (true)
        {
            auto tok = lexer.front();

            if (tok.type == TokenType::bar)
            {
                lexer.pop();
                parse_rhs_1(lexer, lhs, g, manager, productions);
            }
            else if (tok.type == TokenType::semicolon)
            {
                lexer.pop();
                break;
            }
            else
            {
                unexpected_token(tok, lexer, "`|' or `;'");
            }
        }
    }
    
    static void parse_rhs_1(Lexer &lexer, Symbol *lhs, Grammar &g, SymbolManager &manager, std::vector<std::unique_ptr<Production>> &productions)
    {
        std::vector<Symbol *> rhs;

        while (true)
        {
            auto tok = lexer.front();

            if (tok.type == TokenType::ident)
            {
                auto s = manager.get_symbol(tok.value);

                if (s == nullptr)
                {
                    std::ostringstream msg;
                    msg << "error: unknown symbol `" << tok.value << "' at line " << lexer.line() << ".\n";
                    throw std::runtime_error { msg.str() };
                }

                rhs.push_back(s);

                lexer.pop();
            }
            else if (tok.type == TokenType::square_start)
            {
                lexer.pop();
                break;
            }
            else
            {
                unexpected_token(tok, lexer, "an identifier or `['");
            }
        }

        auto name = lexer.front();

        if (name.type != TokenType::ident)
        {
            unexpected_token(name, lexer, "an identifier");
        }

        lexer.pop();
        consume_token(lexer, TokenType::square_end, "`]'");
                
        auto p = std::make_unique<Production>(name.value, lhs, rhs);

        g.productions().push_back(p.get());

        productions.push_back(std::move(p));
    }
    
}
