module;

#include <type_traits>

export module utility.meta.type_traits.all_same;

namespace util::meta {

export template <typename...>
struct all_same;

template <>
struct all_same<> {
    constexpr static bool value = true;
};

template <typename T, typename... Ts>
struct all_same<T, Ts...> {
    constexpr static bool value = std::conjunction_v<std::is_same<T, Ts>...>;
};

export template <typename... Ts>
constexpr inline bool all_same_v = all_same<Ts...>::value;

}   // namespace util::meta
