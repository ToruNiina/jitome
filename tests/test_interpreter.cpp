#include "jitome/ast.hpp"
#include "jitome/eval.hpp"
#include "jitome/interpreter.hpp"
#include <boost/ut.hpp>
#include <iostream>

int main()
{
    using namespace boost::ut::literals;
    using namespace std::literals::string_literals;
    using namespace std::literals::string_view_literals;

    "add"_test = []
    {
        jitome::Node root{
            jitome::NodeFunction{
                "add",
                std::vector{"a"s, "b"s},
                jitome::Node{jitome::NodeExpression{"+"sv,
                    jitome::NodeVariable{"a"},
                    jitome::NodeVariable{"b"}
                }}
            }
        };

        jitome::Interpreter<double, double, double> add(std::move(root));

        boost::ut::expect(3.14 + 2.71 == add(3.14, 2.71));
    };

    "dep"_test = []
    {
        jitome::Node root{
            jitome::NodeFunction{
                "dep",
                std::vector{"a"s, "b"s, "c"s},
                jitome::Node{jitome::NodeExpression{"*"sv,
                    jitome::NodeVariable{"a"},
                    jitome::NodeExpression{"+"sv,
                        jitome::NodeVariable{"b"},
                        jitome::NodeVariable{"c"},
                    }
                }}
            }
        };

        jitome::Interpreter<double, double, double, double> dep(std::move(root));

        boost::ut::expect(2.0 * (3.14 + 2.71) == dep(2.0, 3.14, 2.71));
    };
}
