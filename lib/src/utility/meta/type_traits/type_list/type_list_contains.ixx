module;

#include <type_traits>

export module ddge.utility.meta.type_traits.type_list.type_list_contains;

import ddge.utility.meta.type_traits.type_list.type_list_push_back;

namespace ddge::util::meta {

export template <typename TypeList_T, typename T>
struct type_list_contains;

template <template <typename...> typename TypeList_T, typename... Ts, typename T>
struct type_list_contains<TypeList_T<Ts...>, T>
    : std::disjunction<std::is_same<Ts, T>...> {};

export template <typename TypeList_T, typename T>
inline constexpr bool type_list_contains_v = type_list_contains<TypeList_T, T>::value;

}   // namespace ddge::util::meta
