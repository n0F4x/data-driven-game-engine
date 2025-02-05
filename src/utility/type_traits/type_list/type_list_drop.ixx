module;

#include <cstdint>

export module utility.type_traits.type_list.type_list_drop;

namespace util::meta {

export template <typename TypeList_T, size_t N>
struct type_list_drop;

template <typename TypeList_T>
struct type_list_drop<TypeList_T, 0> {
    using type = TypeList_T;
};

template <template <typename...> typename TypeList_T, typename T, typename... Ts, size_t N>
    requires(sizeof...(Ts) + 1 >= N && N != 0)
struct type_list_drop<TypeList_T<T, Ts...>, N>
    : type_list_drop<TypeList_T<Ts...>, N - 1> {};

export template <typename TypeList_T, size_t N>
using type_list_drop_t = typename type_list_drop<TypeList_T, N>::type;

}   // namespace util::meta
