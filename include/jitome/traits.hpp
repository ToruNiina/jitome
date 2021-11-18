#ifndef JITOME_TRAITS_HPP
#define JITOME_TRAITS_HPP
#include <type_traits>

namespace jitome
{

template<typename T, typename U>
constexpr inline bool is_type = std::is_same_v<std::remove_cvref_t<T>, U>;

} // jitome
#endif// JITOME_TRAITS_HPP
