#ifndef JITOME_TRAITS_HPP
#define JITOME_TRAITS_HPP
#include <type_traits>

namespace jitome
{

template<typename T>
using remove_cvref_t = std::remove_cv_t<std::remove_reference_t<T>>;

template<typename T, typename U>
constexpr inline bool is_type = std::is_same_v<std::remove_cvref_t<T>, U>;

} // jitome
#endif// JITOME_TRAITS_HPP
