#ifndef JITOME_PARSER_HPP
#define JITOME_PARSER_HPP
#include "ast.hpp"
#include "tokenizer.hpp"
#include <memory>
#include <string>
#include <sstream>
#include <string_view>
#include <deque>
#include <cassert>
#include <cctype>

namespace jitome
{
Result<Node> parse_expr(std::deque<Token>& tokens);

inline Result<Node> parse_primary(std::deque<Token>& tokens)
{
    if(tokens.front().kind == TokenKind::LeftBracket)
    {
        tokens.pop_front();
        auto expr = parse_expr(tokens);
        if(tokens.front().kind == TokenKind::RightBracket)
        {
            tokens.pop_front();
            return expr;
        }
        else
        {
            if(tokens.empty())
            {
                return err("parse_primary: expected right bracket `)`, but EOF is found");
            }
            else
            {
                return err(make_error_message(
                    "parse_primary: expected right bracket `)`, but found:",
                    tokens.front()));
            }
        }
    }
    else if(tokens.front().kind == TokenKind::Immediate)
    {
        std::string imm_str(tokens.front().str);
        std::istringstream iss(imm_str);
        double imm = 0.0;
        iss >> imm;

        tokens.pop_front();
        return ok(Node{NodeImmediate{imm}});
    }
    return err(make_error_message("parse_primary: unexpected token appeared",
                                  tokens.front()));
}

inline Result<Node> parse_mul(std::deque<Token>& tokens)
{
    auto lhs = parse_primary(tokens);

    if(lhs.is_err())
    {
        return lhs;
    }

    while(not tokens.empty())
    {
        if(tokens.front().kind == TokenKind::Operator && tokens.front().str == "*")
        {
            tokens.pop_front();
            auto rhs = parse_primary(tokens);
            if(rhs.is_err())
            {
                return rhs;
            }

            lhs = Node{NodeMultiplication{
                std::move(lhs.as_val()), std::move(rhs.as_val())
            }};
        }
        else if(tokens.front().kind == TokenKind::Operator && tokens.front().str == "/")
        {
            tokens.pop_front();
            auto rhs = parse_primary(tokens);
            if(rhs.is_err())
            {
                return rhs;
            }

            lhs = Node{NodeDivision{
                std::move(lhs.as_val()), std::move(rhs.as_val())
            }};
        }
        else
        {
            return lhs;
        }
    }
    return lhs;
}

inline Result<Node> parse_expr(std::deque<Token>& tokens)
{
    auto lhs = parse_mul(tokens);

    if(lhs.is_err())
    {
        return lhs;
    }

    while(not tokens.empty())
    {
        if(tokens.front().kind == TokenKind::Operator && tokens.front().str == "+")
        {
            tokens.pop_front(); // +
            auto rhs = parse_mul(tokens);
            if(rhs.is_err())
            {
                return rhs;
            }

            lhs = Node{NodeAddition{
                std::move(lhs.as_val()), std::move(rhs.as_val())
            }};
        }
        else if(tokens.front().kind == TokenKind::Operator && tokens.front().str == "-")
        {
            tokens.pop_front(); // -
            auto rhs = parse_mul(tokens);
            if(rhs.is_err())
            {
                return rhs;
            }

            lhs = Node{NodeSubtraction{
                std::move(lhs.as_val()), std::move(rhs.as_val())
            }};
        }
        else
        {
            return lhs;
        }
    }
    return lhs;
}

inline Result<Node> parse(std::deque<Token> tokens)
{
    auto root = parse_expr(tokens);
    return root;
}

} // jitome
#endif// JITOME_PARSER_HPP
