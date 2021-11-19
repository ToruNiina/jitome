#ifndef JITOME_AST_HPP
#define JITOME_AST_HPP
#include "traits.hpp"
#include <memory>
#include <string>
#include <variant>

namespace jitome
{

struct Node;

struct NodeVariable
{
    std::string name;
};
struct NodeImmediate
{
    double value;
};

template<typename Func, std::size_t N>
struct NodeExpression
{
    using func_type = Func;
    static constexpr inline std::size_t size = N;
    std::array<std::unique_ptr<Node>, N> operands;
};
template<typename T> struct is_expr_node : std::false_type {};
template<typename F, std::size_t N>
struct is_expr_node<NodeExpression<F, N>> : std::true_type {};
template<typename T>
constexpr inline bool is_expr_node_v = is_expr_node<remove_cvref_t<T>>::value;

#define JITOME_DEFINE_BINARY_OPERATION_FUNCTION(name, expr)      \
    struct name                                                  \
    {                                                            \
        static inline double invoke(double x, double y) noexcept \
        {return (expr);}                                         \
    }

JITOME_DEFINE_BINARY_OPERATION_FUNCTION(Addition,       (x+y));
JITOME_DEFINE_BINARY_OPERATION_FUNCTION(Subtraction,    (x-y));
JITOME_DEFINE_BINARY_OPERATION_FUNCTION(Multiplication, (x*y));
JITOME_DEFINE_BINARY_OPERATION_FUNCTION(Division,       (x/y));
#undef JITOME_DEFINE_BINARY_OPERATION_FUNCTION

#define JITOME_DEFINE_UNARY_OPERATION_FUNCTION(name, expr) \
    struct name                                            \
    {                                                      \
        static inline double invoke(double x) noexcept     \
        {return (expr);}                                   \
    }

JITOME_DEFINE_UNARY_OPERATION_FUNCTION(Negation, (-x));
#undef JITOME_DEFINE_UNARY_OPERATION_FUNCTION

struct Node
{
    std::variant<
        NodeVariable,
        NodeImmediate,
        NodeExpression<Addition,       2>,
        NodeExpression<Subtraction,    2>,
        NodeExpression<Multiplication, 2>,
        NodeExpression<Division,       2>,
        NodeExpression<Negation,       1>
        > node;
};

template<typename T>
std::unique_ptr<Node> make_node_ptr(T&& n)
{
    return std::make_unique<Node>(Node{std::forward<T>(n)});
}

} // jitome
#endif// JITOME_AST_HPP
