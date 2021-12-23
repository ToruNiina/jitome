#ifndef JITOME_JIT_HPP
#define JITOME_JIT_HPP
#include "ast.hpp"
#include "parser.hpp"
#include "tokenizer.hpp"
#include "util.hpp"
#include "xbyak.h"

#include <cassert>

#ifdef XBYAK32
#  error "32-bit environment is not supported"
#endif

#ifdef XBYAK64_WIN
#  error "Windows is not supported."
#endif

namespace jitome
{

template<typename Ret, typename ... Args>
struct JitCompiler : public Xbyak::CodeGenerator
{
  public:

    using func_ptr = Ret (*)(Args...);

  public:

    JitCompiler(std::string code)
        : f_(nullptr)
    {
        auto tks = tokenize(code);
        if(tks.is_err())
        {
            throw std::runtime_error(tks.as_err().msg);
        }
        auto prs = parse(tks.as_val());
        if(prs.is_err())
        {
            throw std::runtime_error(prs.as_err().msg);
        }
        this->compile(std::move(prs.as_val()));
    }

    JitCompiler(Node root)
        : f_(nullptr)
    {
        this->compile(std::move(root));
    }

    Ret operator()(Args... arguments)
    {
        assert(f_ != nullptr);
        return f_(arguments...);
    }

  private:

    // argumnet register
    // - rdi, rsi, rdx, rcx, r8, r9
    // - xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7
    //
    // return register:
    // - rax,  rdx
    // - xmm0, xmm1

    void compile(Node root)
    {
        auto func = std::get<NodeFunction>(std::move(root.node));
        // assuming func has enough small number of argument

        std::map<std::string, std::size_t> narg;
        for(std::size_t i=0; i<func.args.size(); ++i)
        {
            narg[func.args.at(i)] = i;
        }

        push(rbp); // prologue
        mov(rbp, rsp);

        int stk = static_cast<int>(narg.size());
        std::visit([&stk, &narg, this](const auto& n) {
                return this->expand_recursively(stk, narg, n);
            }, func.body->node);

        stk -= 1;
        movsd(xmm0, Xbyak::Xmm(stk));

        mov(rsp, rbp);
        pop(rbp); // epilogue
        ret();

        this->f_ = this->getCode<func_ptr>();
    }

    template<typename T>
    void expand_recursively(int& stk, const std::map<std::string, std::size_t>& narg, const T& node)
    {
        if constexpr (is_typeof<T, NodeVariable>)
        {
            if(narg.count(node.name) == 0)
            {
                throw std::runtime_error("variable definition is currently not supported");
            }
            movsd(Xbyak::Xmm(stk), Xbyak::Xmm(narg.at(node.name)));
            stk += 1;
        }
        else if constexpr (is_typeof<T, NodeImmediate>)
        {
            auto casted_to_int = bit_cast<std::uint64_t>(node.value);
            mov  (rax,             casted_to_int);
            sub  (rsp,             sizeof(std::uint64_t));
            mov  (ptr[rsp],        rax);
            movsd(Xbyak::Xmm(stk), ptr[rsp]);
            stk += 1;
        }
        else if constexpr (is_expr_node_v<T>)
        {
            if constexpr(T::size == 1)
            {
                std::visit([&stk, &narg, this](const auto& n) {
                        return this->expand_recursively(stk, narg, n);
                    }, node.operands[0]->node);
                stk -= 1;
                movsd(xmm15, Xbyak::Xmm(stk));
            }
            else
            {
                std::visit([&stk, &narg, this](const auto& n) {
                        return this->expand_recursively(stk, narg, n);
                    }, node.operands[0]->node);
                std::visit([&stk, &narg, this](const auto& n) {
                        return this->expand_recursively(stk, narg, n);
                    }, node.operands[1]->node);
                stk -= 1;
                movsd(xmm15, Xbyak::Xmm(stk));
                stk -= 1;
                movsd(xmm14, Xbyak::Xmm(stk));
            }
            if constexpr (std::is_same_v<T, NodeExpression<Addition, 2>>)
            {
                addsd(xmm15, xmm14);
            }
            else if constexpr (std::is_same_v<T, NodeExpression<Subtraction, 2>>)
            {
                subsd(xmm15, xmm14);
            }
            else if constexpr (std::is_same_v<T, NodeExpression<Multiplication, 2>>)
            {
                mulsd(xmm15, xmm14);
            }
            else if constexpr (std::is_same_v<T, NodeExpression<Division, 2>>)
            {
                divsd(xmm15, xmm14);
            }
            else if constexpr (std::is_same_v<T, NodeExpression<Negation, 1>>)
            {
                movsd(xmm14, xmm15);
                pxor (xmm15, xmm15);
                subsd(xmm15, xmm14);
            }
            movsd(Xbyak::Xmm(stk), xmm15);
            stk += 1;
        }
        else if constexpr (is_typeof<T, NodeFunction>)
        {
            throw std::runtime_error("function call is not supported");
        }
        else
        {
            throw std::runtime_error("unknown node appeared");
        }

        if(stk > 13)
        {
            throw std::runtime_error("jitome: register run out");
        }
    }

  private:

    func_ptr f_;
};

} // jitome
#endif// JITOME_AST_HPP
