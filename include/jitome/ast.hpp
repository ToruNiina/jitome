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

struct NodeExpression
{
    static constexpr inline std::size_t size = N;

    std::string_view  function; // or enum?
    std::vector<Node> operands;

    NodeExpression(Ts&&... args)
        : operands{std::forward<Ts>(args)...}
    {}

    bool operator==(const NodeExpression& other) const noexcept
    {
        return (this->function == other.function) &&
               (this->operands == other.operands)
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
    retval += node.function;
    retval += "(";
    bool front = true;
    for(const auto& expr : node.operands)
    {
        if(!front) {retval += ", ";}
        retval += dump(expr);
        front = false;
    }
    retval += ")";
    return retval;
}

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
        NodeExpression,
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
