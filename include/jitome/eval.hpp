#ifndef JITOME_EVAL_HPP
#define JITOME_EVAL_HPP
#include "traits.hpp"
#include "ast.hpp"

#include <limits>
#include <map>
#include <memory>
#include <string>

namespace jitome
{

double evaluate(std::map<std::string, double>& env, const Node& root);

inline double invoke_func(std::map<std::string, double>& env, const NodeExpression& node)
{
    using namespace std::literals::string_view_literals;
    if(node.function == "+"sv)
    {
        if(node.operands.size() != 2)
        {
            throw std::runtime_error("jitome::evaluate: invalid number of operands in `+`");
        }
        return evaluate(env, node.operands.at(0)) + evaluate(env, node.operands.at(1));
    }
    else if(node.function == "-"sv)
    {
        if(node.operands.size() == 1)
        {
            return -evaluate(env, node.operands.at(0));
        }
        else
        {
            if(node.operands.size() != 2)
            {
                throw std::runtime_error("jitome::evaluate: invalid number of operands in `-`");
            }
            return evaluate(env, node.operands.at(0)) -
                   evaluate(env, node.operands.at(1));
        }
    }
    else if(node.function == "*"sv)
    {
        if(node.operands.size() != 2)
        {
            throw std::runtime_error("jitome::evaluate: invalid number of operands in `*`");
        }
        return evaluate(env, node.operands.at(0)) *
               evaluate(env, node.operands.at(1));
    }
    else if(node.function == "/"sv)
    {
        if(node.operands.size() != 2)
        {
            throw std::runtime_error("jitome::evaluate: invalid number of operands in `/`");
        }
        return evaluate(env, node.operands.at(0)) /
               evaluate(env, node.operands.at(1));
    }
    else
    {
        throw std::runtime_error("jitome::evaluate: unknown function name: " + std::string(node.function));
    }
}

inline double evaluate(std::map<std::string, double>& env, const Node& root)
{
    return std::visit([&env] (const auto& node) {
        if constexpr (is_typeof<decltype(node), NodeVariable>)
        {
            return env.at(node.name);
        }
        else if constexpr (is_typeof<decltype(node), NodeImmediate>)
        {
            return node.value;
        }
        else if constexpr (is_typeof<decltype(node), NodeExpression>)
        {
            return invoke_func(env, node);
        }
        else if constexpr (is_typeof<decltype(node), NodeFunction>)
        {
            return evaluate(env, node.body);
        }
        else
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
    }, root.node);
}

} // jitome
#endif// JITOME_EVAL_HPP
