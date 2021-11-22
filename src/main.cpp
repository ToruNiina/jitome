#include "jitome/ast.hpp"
#include "jitome/eval.hpp"
#include <iostream>

int main()
{
    jitome::Node root{
        jitome::NodeExpression<jitome::Addition, 2>{
            {{jitome::make_node_ptr(jitome::NodeImmediate{3.14}),
              jitome::make_node_ptr(jitome::NodeImmediate{2.71})}}
        }
    };

    std::map<std::string, double> env;

    std::cout << jitome::evaluate(env, root) << std::endl;
}
