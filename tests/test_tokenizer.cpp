#include <iostream>
#include <jitome/tokenizer.hpp>
#include <boost/ut.hpp>

int main()
{
    using namespace boost::ut::literals;

    "is_chars()"_test = []
    {
        const std::string test("foobar");
        boost::ut::expect(jitome::is_chars(test.begin(), test.end(), "fo"));
        boost::ut::expect(jitome::is_chars(test.begin(), test.end(), "foo"));
        boost::ut::expect(jitome::is_chars(test.begin(), test.end(), "foobar"));

        boost::ut::expect(!jitome::is_chars(test.begin(), test.end(), "b"));
        boost::ut::expect(!jitome::is_chars(test.begin(), test.end(), "bar"));
        boost::ut::expect(!jitome::is_chars(test.begin(), test.end(), "qux"));
    };

    "is_newline"_test = []
    {
        const std::string test1("\n");
        const std::string test2("\r\n");
        const std::string test3("\r");
        boost::ut::expect( jitome::is_newline(test1.begin(), test1.end()));
        boost::ut::expect( jitome::is_newline(test2.begin(), test2.end()));
        boost::ut::expect(!jitome::is_newline(test3.begin(), test3.end()));
    };

    "skip_newline"_test = []
    {
        const std::string test1("\nA");
        const std::string test2("\r\nA");
        const std::string test3("\rA");

        const auto actual1 = jitome::skip_newline(test1.begin(), test1.end());
        const auto actual2 = jitome::skip_newline(test2.begin(), test2.end());
        const auto actual3 = jitome::skip_newline(test3.begin(), test3.end());

        boost::ut::expect(actual1 != test1.end() && *actual1 == 'A');
        boost::ut::expect(actual2 != test2.end() && *actual2 == 'A');
        boost::ut::expect(actual3 != test3.end() && *actual3 == '\r');
    };

    "skip_whitespace"_test = []
    {
        const std::string test1(" A");
        const std::string test2("\tA");
        const std::string test3("   \t \t\t  A");

        const auto actual1 = jitome::skip_whitespace(test1.begin(), test1.end());
        const auto actual2 = jitome::skip_whitespace(test2.begin(), test2.end());
        const auto actual3 = jitome::skip_whitespace(test3.begin(), test3.end());

        boost::ut::expect(actual1 != test1.end() && *actual1 == 'A');
        boost::ut::expect(actual2 != test2.end() && *actual2 == 'A');
        boost::ut::expect(actual3 != test3.end() && *actual3 == 'A');
    };

    "skip_comment_line"_test = []
    {
        const std::string test1("// A\nB");
        const std::string test2("// A  B");
        const std::string test3("/**/ A  B");

        const auto actual1 = jitome::skip_comment_line(test1.begin(), test1.end());
        const auto actual2 = jitome::skip_comment_line(test2.begin(), test2.end());
        const auto actual3 = jitome::skip_comment_line(test3.begin(), test3.end());

        boost::ut::expect(actual1 != test1.end() && *actual1 == 'B');
        boost::ut::expect(actual2 == test2.end());
        boost::ut::expect(actual3 != test3.end() && *actual3 == '/');
    };

    "skip_comment"_test = []
    {
        const std::string test1("/**/A");
        const std::string test2("/* hoge \n fuga */A");
        const std::string test3("//\nA");

        const auto actual1 = jitome::skip_comment(test1.begin(), test1.end());
        const auto actual2 = jitome::skip_comment(test2.begin(), test2.end());
        const auto actual3 = jitome::skip_comment(test3.begin(), test3.end());

        boost::ut::expect(actual1 != test1.end() && *actual1 == 'A');
        boost::ut::expect(actual2 != test2.end() && *actual2 == 'A');
        boost::ut::expect(actual3 != test3.end() && *actual3 == '/');
    };

    "skip_negligible"_test = []
    {
        const std::string test1(" /**/ \t A");
        const std::string test2("/* hoge \n fuga */ // \n A");
        const std::string test3("A");

        const auto actual1 = jitome::skip_negligible(test1.begin(), test1.end());
        const auto actual2 = jitome::skip_negligible(test2.begin(), test2.end());
        const auto actual3 = jitome::skip_negligible(test3.begin(), test3.end());

        boost::ut::expect(actual1 != test1.end() && *actual1 == 'A');
        boost::ut::expect(actual2 != test2.end() && *actual2 == 'A');
        boost::ut::expect(actual3 != test3.end() && *actual3 == 'A');
    };

    return 0;
}
