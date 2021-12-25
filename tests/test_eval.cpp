#include "jitome/ast.hpp"
#include "jitome/eval.hpp"
#include <boost/ut.hpp>
#include <iostream>

int main()
{
    using namespace boost::ut::literals;
    using namespace std::literals::string_view_literals;

    "add"_test = []
    {
        jitome::Node root{
            jitome::NodeExpression{"+"sv,
                jitome::NodeImmediate{3.14},
                jitome::NodeImmediate{2.71}
            }
        };

        std::map<std::string, double> env;
        boost::ut::expect(3.14 + 2.71 == jitome::evaluate(env, root));
    };

    "dep"_test = []
    {
        jitome::Node root{
            jitome::NodeExpression{"*"sv,
                jitome::NodeImmediate{2.0},
                jitome::NodeExpression{"+"sv,
                    jitome::NodeImmediate{3.14},
                    jitome::NodeImmediate{2.71}
                }
            }
        };

        std::map<std::string, double> env;
        boost::ut::expect(2.0 * (3.14 + 2.71) == jitome::evaluate(env, root));
    };
}
