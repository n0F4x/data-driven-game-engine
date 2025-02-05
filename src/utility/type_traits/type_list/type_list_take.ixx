module;

#include <cstdint>

export module utility.type_traits.type_list.type_list_take;

import utility.type_traits.type_list.type_list_push_front;

namespace util::meta {

export template <typename TypeList_T, size_t N>
struct type_list_take;

template <template <typename...> typename TypeList_T, typename... Ts>
struct type_list_take<TypeList_T<Ts...>, 0> {
    using type = TypeList_T<>;
};

template <template <typename...> typename TypeList_T, typename T, typename... Ts, size_t N>
    requires(sizeof...(Ts) >= N && N != 0)
struct type_list_take<TypeList_T<T, Ts...>, N>
    : type_list_push_front<typename type_list_take<TypeList_T<Ts...>, N - 1>::type, T> {};

export template <typename TypeList_T, size_t N>
using type_list_take_t = typename type_list_take<TypeList_T, N>::type;

}   // namespace util::meta
