module;

#include <type_traits>

export module utility.meta.type_traits.type_list.type_list_all_same;

import utility.meta.type_traits.all_same;

namespace util::meta {

export template <typename TypeList_T>
struct type_list_all_same;

template <template <typename> typename TypeList_T, typename... Ts>
struct type_list_all_same<TypeList_T<Ts...>> : std::bool_constant<all_same_v<Ts...>> {};

export template <typename TypeList_T>
inline constexpr bool type_list_all_same_v = type_list_all_same<TypeList_T>::value;

}   // namespace util::meta
