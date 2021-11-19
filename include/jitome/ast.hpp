#ifndef JITOME_AST_HPP
#define JITOME_AST_HPP
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
    inline constexpr std::size_t size = N;
    std::array<std::unique_ptr<Node>, N> operands;
};
template<typename T> struct is_expr_node : std::false_type {};
template<typename F, std::size_t N>
struct is_expr_node<NodeExpression<F, N>> : std::true_type {};
template<typename T>
constexpr inline bool is_expr_node_v = is_expr_node<remove_cvref_t<T>>::value;

struct addition
{
    static inline double invoke(double x, double y) noexcept {return x + y;}
};
struct subtraction
{
    static inline double invoke(double x, double y) noexcept {return x - y;}
};
struct multiplication
{
    static inline double invoke(double x, double y) noexcept {return x * y;}
};
struct division
{
    static inline double invoke(double x, double y) noexcept {return x / y;}
};
struct negation
{
    static inline double invoke(double x) noexcept {return -x;}
};

struct Node
{
    std::variant<
        node_variable,
        node_immediate,
        node_expression<addition,       2>,
        node_expression<subtraction,    2>,
        node_expression<multiplication, 2>,
        node_expression<division,       2>,
        node_expression<negation,       1>
        > node;
};

template<typename T>
std::unique_ptr<Node> make_node_ptr(T&& n)
{
    return std::make_unique<Node>(Node{std::forward<T>(n)});
}

} // jitome
#endif// JITOME_AST_HPP
