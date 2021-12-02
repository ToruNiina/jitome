#ifndef JITOME_AST_HPP
#define JITOME_AST_HPP
#include "traits.hpp"
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace jitome
{

struct Node;
bool operator==(const Node&, const Node&) noexcept;
bool operator!=(const Node&, const Node&) noexcept;
std::string dump(const Node&);

struct NodeVariable
{
    std::string name;

    bool operator==(const NodeVariable& other) const noexcept
    {
        return this->name == other.name;
    }
    bool operator!=(const NodeVariable& other) const noexcept
    {
        return this->name != other.name;
    }
};
inline std::string dump(const NodeVariable& node)
{
    return std::string("Var{") + node.name + "}";
}

struct NodeImmediate
{
    double value;

    bool operator==(const NodeImmediate& other) const noexcept
    {
        return this->value == other.value;
    }
    bool operator!=(const NodeImmediate& other) const noexcept
    {
        return this->value != other.value;
    }
};
inline std::string dump(const NodeImmediate& node)
{
    return std::string("Imm{") + std::to_string(node.value) + "}";;
}

template<typename Func, std::size_t N>
struct NodeExpression
{
    using func_type = Func;
    static constexpr inline std::size_t size = N;

    std::array<std::unique_ptr<Node>, N> operands;

    template<typename ... Ts>
    NodeExpression(Ts&&... args)
        : operands{{std::make_unique<Node>(Node{std::forward<Ts>(args)}) ...}}
    {}

    bool operator==(const NodeExpression& other) const noexcept
    {
        for(std::size_t i=0; i<N; ++i)
        {
            if(this->operands[i] == nullptr)
            {
                if(other.operands[i] != nullptr)
                {
                    return false;
                }
            }
            else
            {
                if(other.operands[i] == nullptr ||
                   *(this->operands[i]) != *(other.operands[i]))
                {
                    return false;
                }
            }
        }
        return true;
    }
    bool operator!=(const NodeExpression& other) const noexcept
    {
        return !(*this == other);
    }
};

template<typename Func, std::size_t N>
inline std::string dump(const NodeExpression<Func, N>& node)
{
    std::string retval("Expr{");
    retval += Func::dump();
    retval += "(";
    bool front = true;
    for(const auto& expr : node.operands)
    {
        if(!front) {retval += ", ";}
        retval += dump(*expr);
        front = false;
    }
    retval += ")";
    return retval;
}

template<typename T> struct is_expr_node : std::false_type {};
template<typename F, std::size_t N>
struct is_expr_node<NodeExpression<F, N>> : std::true_type {};
template<typename T>
constexpr inline bool is_expr_node_v = is_expr_node<remove_cvref_t<T>>::value;

#define JITOME_DEFINE_BINARY_OPERATION_FUNCTION(name, expr)        \
    struct name                                                    \
    {                                                              \
        static inline double invoke(double x, double y) noexcept   \
        {return (expr);}                                           \
        static inline std::string dump() noexcept {return "name";} \
    };                                                             \
    using Node ## name = NodeExpression<name, 2>;               /**/

JITOME_DEFINE_BINARY_OPERATION_FUNCTION(Addition,       (x+y))
JITOME_DEFINE_BINARY_OPERATION_FUNCTION(Subtraction,    (x-y))
JITOME_DEFINE_BINARY_OPERATION_FUNCTION(Multiplication, (x*y))
JITOME_DEFINE_BINARY_OPERATION_FUNCTION(Division,       (x/y))
#undef JITOME_DEFINE_BINARY_OPERATION_FUNCTION

#define JITOME_DEFINE_UNARY_OPERATION_FUNCTION(name, expr)         \
    struct name                                                    \
    {                                                              \
        static inline double invoke(double x) noexcept             \
        {return (expr);}                                           \
        static inline std::string dump() noexcept {return "name";} \
    };                                                             \
    using Node ## name = NodeExpression<name, 1>;               /**/

JITOME_DEFINE_UNARY_OPERATION_FUNCTION(Negation, (-x))
#undef JITOME_DEFINE_UNARY_OPERATION_FUNCTION

struct NodeFunction
{
    std::string              name;
    std::vector<std::string> args;
    std::unique_ptr<Node>    body;

    bool operator==(const NodeFunction& other) const noexcept
    {
        return this->args == other.args &&
            this->body != nullptr && other.body != nullptr &&
            *(this->body) == *(other.body);
    }
    bool operator!=(const NodeFunction& other) const noexcept
    {
        return !(*this == other);
    }
};

inline std::string dump(const NodeFunction& node)
{
    std::string retval;
    retval += node.name;
    retval += "(";
    bool front = true;
    for(const auto& arg : node.args)
    {
        if(!front) {retval += ", ";}
        retval += arg;
        front = false;
    }
    retval += ") {";
    retval += dump(*node.body);
    retval += "}";
    return retval;
}

struct Node
{
    std::variant<
        NodeVariable,
        NodeImmediate,
        NodeAddition,
        NodeSubtraction,
        NodeMultiplication,
        NodeDivision,
        NodeNegation,
        NodeFunction
        > node;

};

inline std::string dump(const Node& node)
{
    return std::visit([](const auto& n) {return dump(n);}, node.node);
}

inline bool operator==(const Node& lhs, const Node& rhs) noexcept
{
    return lhs.node == rhs.node;
}
inline bool operator!=(const Node& lhs, const Node& rhs) noexcept
{
    return lhs.node != rhs.node;
}

template<typename T>
std::unique_ptr<Node> make_node_ptr(T&& n)
{
    return std::make_unique<Node>(Node{std::forward<T>(n)});
}

} // jitome
#endif// JITOME_AST_HPP
