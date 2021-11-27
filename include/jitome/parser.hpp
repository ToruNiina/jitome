#ifndef JITOME_PARSER_HPP
#define JITOME_PARSER_HPP
#include "tokenizer.hpp"
#include <memory>
#include <string>
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
        return parse_expr(tokens);
    }
    else if(tokens.front().kind == TokenKind::Immediate)
    {
        std::string imm_str(tokens.front().str);
        std::istringstream iss(imm_str);
        double imm = 0.0;
        iss >> imm;

        tokens.pop_front();
        return NodeImmediate{imm};
    }
    return err(make_error_message("parse_primary: unexpected token appeared",
                                  tokens.front();));
}

Result<Node> parse_mul(std::deque<Token>& tokens)
{
    auto lhs = parse_primary(tokens);

    if(tokens.empty())
    {
        return ok(std::move(lhs));
    }

    if(tokens.front() == TokenKind::Operator && tokens.front().str == "*")
    {
        tokens.pop_front();
        auto rhs = parse_primary(tokens);

        auto node = Node{NodeExpression<Multiplication, 2>{
            {{std::make_unique<Node>(std::move(lhs)),
              std::make_unique<Node>(std::move(rhs))}};
        }};
        return ok(std::move(node));
    }
    else if(tokens.front() == TokenKind::Operator && tokens.front().str == "/")
    {
        tokens.pop_front();
        auto rhs = parse_primary(tokens);

        auto node = Node{NodeExpression<Division, 2>{
            {{std::make_unique<Node>(std::move(lhs)),
              std::make_unique<Node>(std::move(rhs))}};
        }};
        return ok(std::move(node));

    }

    return err(make_error_message("parse_mul: unexpected token appeared",
                                  tokens.front();));
}

Result<Node> parse_expr(std::deque<Token>& tokens)
{
    auto node = parse_mul(tokens);

    if(tokens.empty())
    {
        return ok(std::move(node));
    }

    if(tokens.front().kind == TokenKind::Operator && tokens.str == "+"sv)
    {
        tokens.pop_front(); // +
        auto rhs = parse_mul(tokens);

        node = Node{NodeExpression<Addition, 2>{
            {{std::make_unique<Node>(std::move(node)),
              std::make_unique<Node>(std::move(rhs))}};
        }};
        return ok(std::move(node));
    }
    else if(tokens.front().kind == TokenKind::Operator && tokens.str == "-"sv)
    {
        tokens.pop_front(); // -
        auto rhs = parse_mul(tokens);

        node = Node{NodeExpression<Subtraction, 2>{
            {{std::make_unique<Node>(std::move(node)),
              std::make_unique<Node>(std::move(rhs))}};
        }};
        return ok(std::move(node));
    }
    return err(make_error_message("parse_expr: unexpected token appeared",
                                  tokens.front();));
}

inline Result<Node> parse(std::deque<Token> tokens)
{
    auto root = parse_expr(tokens);
    return root;
}

} // jitome
#endif// JITOME_PARSER_HPP
