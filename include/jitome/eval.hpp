#ifndef JITOME_EVAL_HPP
#define JITOME_EVAL_HPP
#include "traits.hpp"
#include "ast.hpp"

#include <memory>
#include <string>

namespace jitome
{

double evaluate(std::map<std::string, double>& env, const Node& root)
{
    return std::visit([&env] (const auto& node) {
        if constexpr (is_typeof<decltype(node), NodeVariable>)
        {
            return env.at(node.name);
        }
        else if constexpr (is_typeof<decltype(node), NodeImmedaite>)
        {
            return node.value;
        }
        else if constexpr (is_expr<decltype(node), 1>)
        {
            return func_type_of<decltype(node)>::invoke(
                    evaluate(node.operands[0])
                );
        }
        else if constexpr (is_expr<decltype(node), 2>)
        {
            return func_type_of<decltype(node)>::invoke(
                    evaluate(node.operands[0]),
                    evaluate(node.operands[1])
                );
        }
        else if constexpr (is_expr<decltype(node), 3>)
        {
            return func_type_of<decltype(node)>::invoke(
                    evaluate(node.operands[0]),
                    evaluate(node.operands[1]),
                    evaluate(node.operands[2])
                );
        }
        else
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
    }, root);
}

} // jitome
#endif// JITOME_EVAL_HPP
