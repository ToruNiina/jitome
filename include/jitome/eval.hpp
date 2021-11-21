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

template<typename F, std::size_t N, std::size_t ... Is>
double invoke_func_impl(std::map<std::string, double>& env,
        const NodeExpression<F, N>& node, std::index_sequence<Is...>)
{
    return F::invoke(evaluate(env, *std::get<Is>(node.operands))...);
}

template<typename F, std::size_t N>
double invoke_func(std::map<std::string, double>& env, const NodeExpression<F, N>& node)
{
    return invoke_func_impl(env, node, std::make_index_sequence<N>{});
}

double evaluate(std::map<std::string, double>& env, const Node& root)
{
    return std::visit([&env] (const auto& node) {
        if constexpr (is_typeof<decltype(node), NodeArgument>)
        {
            return env.at(node.name);
        }
        else if constexpr (is_typeof<decltype(node), NodeImmediate>)
        {
            return node.value;
        }
        else if constexpr (is_expr_node_v<decltype(node)>)
        {
            return invoke_func(env, node);
        }
        else
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
    }, root.node);
}

} // jitome
#endif// JITOME_EVAL_HPP
