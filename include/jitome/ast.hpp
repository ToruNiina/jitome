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
    std::array<std::unique_ptr<Node>, N> operands;
};
template<typename T> struct is_unary_expr  : std::false_type {};
template<typename T> struct is_binary_expr : std::false_type {};
template<typename T> struct is_triary_expr : std::false_type {};
template<typename F> struct is_unary_expr<NodeExpression<F, 1>>   : std::true_type {};
template<typename F> struct is_binary_expr<NodeExpression<F, 2>>  : std::true_type {};
template<typename F> struct is_trinary_expr<NodeExpression<F, 3>> : std::true_type {};

template<typename F, std::size_t N>
using func_type_of = typename NodeExpression<F, N>::func_type;

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



} // jitome
#endif// JITOME_AST_HPP
