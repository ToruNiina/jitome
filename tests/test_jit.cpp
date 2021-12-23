#include "jitome/ast.hpp"
#include "jitome/eval.hpp"
#include "jitome/jit.hpp"
#include <boost/ut.hpp>
#include <iostream>

int main()
{
    using namespace boost::ut::literals;
    using namespace std::literals::string_literals;

    "add"_test = []
    {
        jitome::Node root{
            jitome::NodeFunction{
                "add",
                std::vector{"a"s, "b"s},
                jitome::make_node_ptr(jitome::NodeAddition{
                        jitome::NodeVariable{"a"},
                        jitome::NodeVariable{"b"}
                    }
                )
            }
        };

        jitome::JitCompiler<double(double, double)> add(std::move(root));

        boost::ut::expect(3.14 + 2.71 == add(3.14, 2.71));
    };

    "add1"_test = []
    {
        jitome::JitCompiler<double(double, double)> add("(a, b) {a + b}");

        boost::ut::expect(3.14 + 2.71 == add(3.14, 2.71));
    };

    "dep"_test = []
    {
        jitome::Node root{
            jitome::NodeFunction{
                "dep",
                std::vector{"a"s, "b"s, "c"s},
                jitome::make_node_ptr(jitome::NodeMultiplication{
                    jitome::NodeVariable{"a"},
                    jitome::NodeAddition{
                        jitome::NodeVariable{"b"},
                        jitome::NodeVariable{"c"},
                    }
                })
            }
        };

        jitome::JitCompiler<double(double, double, double)> dep(std::move(root));

        boost::ut::expect(2.0 * (3.14 + 2.71) == dep(2.0, 3.14, 2.71));
    };

    "dep1"_test = []
    {
        jitome::JitCompiler<double(double, double, double)> dep("(a, b, c) {a * (c + b)}");
        boost::ut::expect(2.0 * (3.14 + 2.71) == dep(2.0, 3.14, 2.71));
    };
}
