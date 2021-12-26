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

    "scan_immediate"_test = []
    {
        const auto test1 = std::make_shared<std::string>("1A");
        const auto test2 = std::make_shared<std::string>("1.0123A");
        const auto test3 = std::make_shared<std::string>("1.0123E+10A");

        auto begin1 = test1->begin();
        auto begin2 = test2->begin();
        auto begin3 = test3->begin();

        const auto actual1 = jitome::scan_immediate(begin1, test1->end(), test1).as_val();
        const auto actual2 = jitome::scan_immediate(begin2, test2->end(), test2).as_val();
        const auto actual3 = jitome::scan_immediate(begin3, test3->end(), test3).as_val();

        boost::ut::expect(actual1.kind == jitome::TokenKind::Immediate);
        boost::ut::expect(actual2.kind == jitome::TokenKind::Immediate);
        boost::ut::expect(actual3.kind == jitome::TokenKind::Immediate);

        boost::ut::expect(actual1.str == "1");
        boost::ut::expect(actual2.str == "1.0123");
        boost::ut::expect(actual3.str == "1.0123E+10");

        boost::ut::expect(actual1.begin == 0);
        boost::ut::expect(actual2.begin == 0);
        boost::ut::expect(actual3.begin == 0);

        boost::ut::expect(actual1.len ==  1);
        boost::ut::expect(actual2.len ==  6);
        boost::ut::expect(actual3.len == 10);

        boost::ut::expect(actual1.src == test1);
        boost::ut::expect(actual2.src == test2);
        boost::ut::expect(actual3.src == test3);
    };

    "scan_identifier"_test = []
    {
        const auto test1 = std::make_shared<std::string>("Foo");
        const auto test2 = std::make_shared<std::string>("bar_baz");
        const auto test3 = std::make_shared<std::string>("f123");

        auto begin1 = test1->begin();
        auto begin2 = test2->begin();
        auto begin3 = test3->begin();

        const auto actual1 = jitome::scan_identifier(begin1, test1->end(), test1).as_val();
        const auto actual2 = jitome::scan_identifier(begin2, test2->end(), test2).as_val();
        const auto actual3 = jitome::scan_identifier(begin3, test3->end(), test3).as_val();

        boost::ut::expect(actual1.kind == jitome::TokenKind::Identifier);
        boost::ut::expect(actual2.kind == jitome::TokenKind::Identifier);
        boost::ut::expect(actual3.kind == jitome::TokenKind::Identifier);

        boost::ut::expect(actual1.str == "Foo");
        boost::ut::expect(actual2.str == "bar_baz");
        boost::ut::expect(actual3.str == "f123");

        boost::ut::expect(actual1.begin == 0);
        boost::ut::expect(actual2.begin == 0);
        boost::ut::expect(actual3.begin == 0);

        boost::ut::expect(actual1.len == 3);
        boost::ut::expect(actual2.len == 7);
        boost::ut::expect(actual3.len == 4);

        boost::ut::expect(actual1.src == test1);
        boost::ut::expect(actual2.src == test2);
        boost::ut::expect(actual3.src == test3);
    };

    "scan_operator"_test = []
    {
        const auto test1 = std::make_shared<std::string>("+");
        const auto test2 = std::make_shared<std::string>("-");
        const auto test3 = std::make_shared<std::string>("()");

        auto begin1 = test1->begin();
        auto begin2 = test2->begin();
        auto begin3 = test3->begin();

        const auto actual1 = jitome::scan_operator(begin1, test1->end(), test1).as_val();
        const auto actual2 = jitome::scan_operator(begin2, test2->end(), test2).as_val();
        const auto actual3 = jitome::scan_operator(begin3, test3->end(), test3).as_val();

        boost::ut::expect(actual1.kind == jitome::TokenKind::Operator);
        boost::ut::expect(actual2.kind == jitome::TokenKind::Operator);
        boost::ut::expect(actual3.kind == jitome::TokenKind::LeftParen);

        boost::ut::expect(actual1.str == "+");
        boost::ut::expect(actual2.str == "-");
        boost::ut::expect(actual3.str == "(");

        boost::ut::expect(actual1.begin == 0);
        boost::ut::expect(actual2.begin == 0);
        boost::ut::expect(actual3.begin == 0);

        boost::ut::expect(actual1.len == 1);
        boost::ut::expect(actual2.len == 1);
        boost::ut::expect(actual3.len == 1);

        boost::ut::expect(actual1.src == test1);
        boost::ut::expect(actual2.src == test2);
        boost::ut::expect(actual3.src == test3);

        // ---------------------------------------------------------------------

        const auto test4 = std::make_shared<std::string>("@");
        auto begin4 = test4->begin();
        const auto actual4 = jitome::scan_operator(begin4, test4->end(), test4);
        boost::ut::expect(actual4.is_err());
    };

    "scan_token"_test = []
    {
        const auto test1 = std::make_shared<std::string>("+foo");
        const auto test2 = std::make_shared<std::string>("bar-3");
        const auto test3 = std::make_shared<std::string>("3.14*baz");

        auto begin1 = test1->begin();
        auto begin2 = test2->begin();
        auto begin3 = test3->begin();

        const auto actual1 = jitome::scan_token(begin1, test1->end(), test1).as_val();
        const auto actual2 = jitome::scan_token(begin2, test2->end(), test2).as_val();
        const auto actual3 = jitome::scan_token(begin3, test3->end(), test3).as_val();

        boost::ut::expect(actual1.kind == jitome::TokenKind::Operator);
        boost::ut::expect(actual2.kind == jitome::TokenKind::Identifier);
        boost::ut::expect(actual3.kind == jitome::TokenKind::Immediate);

        boost::ut::expect(actual1.str == "+");
        boost::ut::expect(actual2.str == "bar");
        boost::ut::expect(actual3.str == "3.14");

        boost::ut::expect(actual1.begin == 0);
        boost::ut::expect(actual2.begin == 0);
        boost::ut::expect(actual3.begin == 0);

        boost::ut::expect(actual1.len == 1);
        boost::ut::expect(actual2.len == 3);
        boost::ut::expect(actual3.len == 4);

        boost::ut::expect(actual1.src == test1);
        boost::ut::expect(actual2.src == test2);
        boost::ut::expect(actual3.src == test3);
    };

    "tokenize"_test = []
    {
        {
            const auto actual = jitome::tokenize("foo+bar-1.5");
            boost::ut::expect(actual.is_ok());

            const auto actual1 = actual.as_val();

            boost::ut::expect(actual1.size() == 5);
            boost::ut::expect(actual1.at(0).kind == jitome::TokenKind::Identifier);
            boost::ut::expect(actual1.at(1).kind == jitome::TokenKind::Operator);
            boost::ut::expect(actual1.at(2).kind == jitome::TokenKind::Identifier);
            boost::ut::expect(actual1.at(3).kind == jitome::TokenKind::Operator);
            boost::ut::expect(actual1.at(4).kind == jitome::TokenKind::Immediate);

            boost::ut::expect(actual1.at(0).str == "foo");
            boost::ut::expect(actual1.at(1).str == "+");
            boost::ut::expect(actual1.at(2).str == "bar");
            boost::ut::expect(actual1.at(3).str == "-");
            boost::ut::expect(actual1.at(4).str == "1.5");
        }

        {
            const auto actual = jitome::tokenize("(foo, bar) {return foo * 2 + bar}");
            boost::ut::expect(actual.is_ok());

            const auto actual1 = actual.as_val();

            boost::ut::expect(actual1.size() == 13);
            boost::ut::expect(actual1.at( 0).kind == jitome::TokenKind::LeftParen);
            boost::ut::expect(actual1.at( 1).kind == jitome::TokenKind::Identifier);
            boost::ut::expect(actual1.at( 2).kind == jitome::TokenKind::Comma);
            boost::ut::expect(actual1.at( 3).kind == jitome::TokenKind::Identifier);
            boost::ut::expect(actual1.at( 4).kind == jitome::TokenKind::RightParen);
            boost::ut::expect(actual1.at( 5).kind == jitome::TokenKind::LeftCurly);
            boost::ut::expect(actual1.at( 6).kind == jitome::TokenKind::Keyword);
            boost::ut::expect(actual1.at( 7).kind == jitome::TokenKind::Identifier);
            boost::ut::expect(actual1.at( 8).kind == jitome::TokenKind::Operator);
            boost::ut::expect(actual1.at( 9).kind == jitome::TokenKind::Immediate);
            boost::ut::expect(actual1.at(10).kind == jitome::TokenKind::Operator);
            boost::ut::expect(actual1.at(11).kind == jitome::TokenKind::Identifier);
            boost::ut::expect(actual1.at(12).kind == jitome::TokenKind::RightCurly);

            boost::ut::expect(actual1.at( 0).str == "("  );
            boost::ut::expect(actual1.at( 1).str == "foo");
            boost::ut::expect(actual1.at( 2).str == ","  );
            boost::ut::expect(actual1.at( 3).str == "bar");
            boost::ut::expect(actual1.at( 4).str == ")"  );
            boost::ut::expect(actual1.at( 5).str == "{"  );
            boost::ut::expect(actual1.at( 6).str == "return");
            boost::ut::expect(actual1.at( 7).str == "foo");
            boost::ut::expect(actual1.at( 8).str == "*"  );
            boost::ut::expect(actual1.at( 9).str == "2"  );
            boost::ut::expect(actual1.at(10).str == "+"  );
            boost::ut::expect(actual1.at(11).str == "bar");
            boost::ut::expect(actual1.at(12).str == "}"  );
        }
    };

    return 0;
}
