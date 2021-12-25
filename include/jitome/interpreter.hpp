#ifndef JITOME_INTERPRETER_HPP
#define JITOME_INTERPRETER_HPP
#include "ast.hpp"
#include "eval.hpp"

#include <limits>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

namespace jitome
{

template<typename Ret, typename ... Args>
struct Interpreter
{
    static_assert(std::conjunction_v<std::is_same<Args, double>...>,
                  "currently, `double` is the only type allowed in jitome.");

    Interpreter(Node root)
        : func_(std::move(root))
    {}

    Ret operator()(Args ... arguments)
    {
        using namespace std::literals::string_literals;

        std::vector<double> args{arguments...};

        const auto& func = std::get<NodeFunction>(func_.node);

        if(args.size() != func.args.size())
        {
            throw std::runtime_error("function `"s + func.name + "` requires "s
                    + std::to_string(func.args.size()) + " arguments, but "s
                    + "only "s + std::to_string(args.size()) + " are provided."s
                    );
        }

        std::map<std::string, double> env;
        for(std::size_t i=0; i<args.size(); ++i)
        {
            env[func.args.at(i)] = args.at(i);
        }

        return evaluate(env, func.body);
    }

  private:
    Node func_;
};


} // jitome
#endif// JITOME_EVAL_HPP
