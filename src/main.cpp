#include "jitome/ast.hpp"
#include "jitome/eval.hpp"
#include "jitome/parser.hpp"
#include "jitome/tokenizer.hpp"
#include <iostream>

int main(int argc, char **argv)
{
    if(argc != 2)
    {
        std::cerr << "usage: ./jitome 1.0 + 3.14 * 2.71" << std::endl;
        return 1;
    }

    const std::string input(argv[1]);

    const auto tokens = jitome::tokenize(input);
    if(tokens.is_err())
    {
        std::cerr << tokens.as_err().msg << std::endl;
        return 1;
    }

    const auto root = jitome::parse(tokens.as_val());
    if(root.is_err())
    {
        std::cerr << root.as_err().msg << std::endl;
        return 1;
    }

    std::map<std::string, double> env;
    std::cout << jitome::evaluate(env, root.as_val()) << std::endl;
    return 0;
}
