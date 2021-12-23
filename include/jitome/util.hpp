#ifndef JITOME_UTIL_HPP
#define JITOME_UTIL_HPP
#include <type_traits>
#include <cstring>

namespace jitome
{

template<typename To, typename From>
To bit_cast(const From& from)
{
    static_assert(sizeof(From) == sizeof(To));
    To to;

    std::memcpy(reinterpret_cast<char*      >(std::addressof(to)),
                reinterpret_cast<const char*>(std::addressof(from)),
                sizeof(From));
    return to;
}


} // jitome
#endif// JITOME_UTIL_HPP
