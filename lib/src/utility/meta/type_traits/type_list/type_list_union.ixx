module;

#include <type_traits>

export module utility.meta.type_traits.type_list.type_list_union;

import utility.meta.type_traits.type_list.type_list_contains;
import utility.meta.type_traits.type_list.type_list_concat;
import utility.meta.type_traits.type_list.type_list_filter;

namespace util::meta {

export template <typename...>
struct type_list_union;

template <template <typename...> typename TypeList_T, typename... Ts>
struct type_list_union<TypeList_T<Ts...>> {
    using type = TypeList_T<Ts...>;
};

template <template <typename...> typename TypeList_T, typename... Ts0, typename... Ts1>
struct type_list_union<TypeList_T<Ts0...>, TypeList_T<Ts1...>> {
    template <typename T>
    struct ContainsTypeFromFront
        : std::bool_constant<!type_list_contains_v<TypeList_T<Ts0...>, T>> {};

    using filtered_back_t = type_list_filter_t<TypeList_T<Ts1...>, ContainsTypeFromFront>;

    using type = type_list_concat_t<TypeList_T<Ts0...>, filtered_back_t>;
};

template <typename TypeList0_T, typename TypeList1_T, typename TypeList2_T, typename... TypeLists_T>
struct type_list_union<TypeList0_T, TypeList1_T, TypeList2_T, TypeLists_T...> {
    using type = typename type_list_union<
        typename type_list_union<TypeList0_T, TypeList1_T>::type,
        TypeList2_T,
        TypeLists_T...>::type;
};

export template <typename... TypeLists_T>
    requires(sizeof...(TypeLists_T) != 0)
using type_list_union_t = typename type_list_union<TypeLists_T...>::type;

}   // namespace util::meta
