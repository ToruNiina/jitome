#include <iostream>

#include "jitome/ast.hpp"
#include "jitome/parser.hpp"
#include <boost/ut.hpp>

int main()
{
    using namespace boost::ut::literals;

    "add"_test = []
    {
        jitome::Node expect{
            jitome::NodeAddition{
                jitome::NodeImmediate{3.14},
                jitome::NodeImmediate{2.71}
            }
        };

        auto tks = jitome::tokenize("3.14 + 2.71");
        boost::ut::expect(tks.is_ok());
        if(tks.is_err())
        {
            std::cout << tks.as_err().msg << std::endl;
        }

        auto actual = jitome::parse(std::move(tks.as_val()));
        boost::ut::expect(actual.is_ok());
        if(actual.is_err())
        {
            std::cout << actual.as_err().msg << std::endl;
        }

        boost::ut::expect(expect == actual.as_val());
    };

    "add"_test = []
    {
        jitome::Node expect{
            jitome::NodeAddition{
                jitome::Node{jitome::NodeAddition{
                        jitome::NodeImmediate{1.00},
                        jitome::NodeImmediate{3.14}
                    }},
                jitome::NodeImmediate{2.71}
            }
        };

        auto tks = jitome::tokenize("1 + 3.14 + 2.71");
        boost::ut::expect(tks.is_ok());
        if(tks.is_err())
        {
            std::cout << tks.as_err().msg << std::endl;
        }

        auto actual = jitome::parse(std::move(tks.as_val()));
        boost::ut::expect(actual.is_ok());
        if(actual.is_err())
        {
            std::cout << actual.as_err().msg << std::endl;
        }

        boost::ut::expect(expect == actual.as_val());
    };

    "dep"_test = []
    {
        jitome::Node expect{
            jitome::NodeMultiplication{
                jitome::NodeImmediate{2.0},
                jitome::NodeAddition{
                    jitome::NodeImmediate{3.14},
                    jitome::NodeImmediate{2.71}
                }
            }
        };
        auto tks = jitome::tokenize("2 * (3.14 + 2.71)");
        boost::ut::expect(tks.is_ok());
        if(tks.is_err())
        {
            std::cout << tks.as_err().msg << std::endl;
        }
        auto actual = jitome::parse(std::move(tks.as_val()));
        boost::ut::expect(actual.is_ok());
        if(actual.is_err())
        {
            std::cout << actual.as_err().msg << std::endl;
        }

        boost::ut::expect(expect == actual.as_val());

    };
}