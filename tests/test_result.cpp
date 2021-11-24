#include <jitome/result.hpp>
#include <boost/ut.hpp>


int main()
{
    using namespace boost::ut::literals;

    "ok(...)"_test = []
    {
        const auto val1 = jitome::ok(3.14);
        static_assert(std::is_same_v<jitome::remove_cvref_t<decltype(val1)>::value_type, double>);
        boost::ut::expect(val1.is_ok());
        boost::ut::expect(val1.is_val());
        boost::ut::expect(val1.as_val() == 3.14);

        const auto val2 = jitome::ok<int>(3.14);
        static_assert(std::is_same_v<jitome::remove_cvref_t<decltype(val2)>::value_type, int>);
        boost::ut::expect(val2.is_ok());
        boost::ut::expect(val2.is_val());
        boost::ut::expect(val2.as_val() == int(3.14));
    };

    "ok(void)"_test = []
    {
        const auto val1 = jitome::ok();
        static_assert(std::is_same_v<jitome::remove_cvref_t<decltype(val1)>::value_type, void>);
        boost::ut::expect(val1.is_ok());
        boost::ut::expect(val1.is_val());
    };

    "err(...)"_test = []
    {
        const auto val1 = jitome::err("hoge");
        static_assert(std::is_same_v<jitome::remove_cvref_t<decltype(val1)>::value_type, void>);
        boost::ut::expect(!val1.is_ok());
        boost::ut::expect(!val1.is_val());
        boost::ut::expect( val1.is_err());
        boost::ut::expect( val1.as_err().msg == "hoge");

        const auto val2 = jitome::err<int>("piyo");
        static_assert(std::is_same_v<jitome::remove_cvref_t<decltype(val2)>::value_type, int>);
        boost::ut::expect(!val2.is_ok());
        boost::ut::expect(!val2.is_val());
        boost::ut::expect( val2.is_err());
        boost::ut::expect( val2.as_err().msg == "piyo");
    };
    return 0;
}
