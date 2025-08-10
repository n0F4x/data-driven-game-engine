module;

#include <type_traits>

export module ddge.utility.meta.type_traits.all_same;

namespace ddge::util::meta {

export template <typename...>
struct all_same;

template <>
struct all_same<> : std::bool_constant<true> {};

template <typename T, typename... Ts>
struct all_same<T, Ts...> : std::conjunction<std::is_same<T, Ts>...> {};

export template <typename... Ts>
inline constexpr bool all_same_v = all_same<Ts...>::value;

}   // namespace ddge::util::meta
