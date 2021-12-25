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
    if(tokens.front().kind == TokenKind::LeftParen)
    {
        tokens.pop_front();
        auto expr = parse_expr(tokens);
        if(tokens.front().kind == TokenKind::RightParen)
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
    else if(tokens.front().kind == TokenKind::Identifier)
    {
        std::string ident(tokens.front().str);
        tokens.pop_front();
        return ok(Node{NodeVariable{std::move(ident)}});
    }
    return err(make_error_message("parse_primary: unexpected token appeared",
                                  tokens.front()));
}

inline Result<Node> parse_mul(std::deque<Token>& tokens)
{
    using namespace std::literals::string_view_literals;
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

            lhs = Node{NodeExpression{"*"sv,
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

            lhs = Node{NodeExpression{"/"sv,
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
    using namespace std::literals::string_view_literals;
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

            lhs = Node{NodeExpression{"+"sv,
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

            lhs = Node{NodeExpression{"-"sv,
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

inline Result<Node> parse_funcdef(std::deque<Token>& tokens)
{
    if(tokens.empty())
    {
        return err("No tokens left.");
    }

    NodeFunction defun;

    //XXX: no name here

    // parse (a, b)

    if(tokens.front().kind != TokenKind::LeftParen)
    {
        return err(make_error_message("parse_funcdef: expected left paren, but found: ",
                    tokens.front()));
    }
    tokens.pop_front(); // pop LeftParen

    bool is_first = true;
    while(not tokens.empty())
    {
        if(!is_first)
        {
            if(tokens.front().kind != TokenKind::Comma)
            {
                return err(make_error_message("parse_funcdef: expected comma, but found: ",
                           tokens.front()));
            }
            else
            {
                tokens.pop_front(); // comma
            }
        }
        if(tokens.front().kind != TokenKind::Identifier)
        {
            return err(make_error_message("parse_funcdef: expected identifier, but found: ",
                       tokens.front()));
        }

        defun.args.push_back(std::string(tokens.front().str));
        tokens.pop_front();
        is_first = false;

        if(tokens.front().kind == TokenKind::RightParen)
        {
            tokens.pop_front();
            break;
        }
    }
    if(tokens.empty())
    {
        return err("parse_funcdef: expected right paren, but no tokens left");
    }

    // parse {expr}

    if(tokens.front().kind != TokenKind::LeftCurly)
    {
        return err(make_error_message("parse_funcdef: expected left curly brace, but found: ",
                    tokens.front()));
    }
    tokens.pop_front(); // pop LeftCurly

    auto expr = parse_expr(tokens);
    if(expr.is_err())
    {
        return expr;
    }
    defun.body = std::make_unique<Node>(std::move(expr.as_val()));

    if(tokens.front().kind != TokenKind::RightCurly)
    {
        return err(make_error_message("parse_funcdef: expected right curly brace, but found: ",
                    tokens.front()));
    }
    tokens.pop_front(); // pop RightCurly

    return ok(Node{std::move(defun)});
}

inline Result<Node> parse(std::deque<Token> tokens)
{
    if(tokens.front().kind == TokenKind::LeftParen)
    {
        return parse_funcdef(tokens);
    }
    else
    {
        return parse_expr(tokens);
    }
}

} // jitome
#endif// JITOME_PARSER_HPP
