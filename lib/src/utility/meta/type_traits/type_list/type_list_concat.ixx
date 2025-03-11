module;

#include <tuple>

export module utility.meta.type_traits.type_list.type_list_concat;

namespace util::meta {

export template <typename...>
struct type_list_concat;

template <
    template <typename...> typename TypeList0_T,
    typename... Ts0,
    template <typename...> typename TypeList1_T,
    typename... Ts1>
struct type_list_concat<TypeList0_T<Ts0...>, TypeList1_T<Ts1...>> {
    using type = std::tuple<Ts0..., Ts1...>;
};

template <typename TypeList0_T, typename TypeList1_T, typename TypeList3_T, typename... TypeLists_T>
struct type_list_concat<TypeList0_T, TypeList1_T, TypeList3_T, TypeLists_T...> {
    using type = typename type_list_concat<
        TypeList0_T,
        typename type_list_concat<TypeList1_T, TypeList3_T, TypeLists_T...>::type>::type;
};

export template <typename... TypeLists_T>
    requires(sizeof...(TypeLists_T) >= 2)
using type_list_concat_t = typename type_list_concat<TypeLists_T...>::type;

}   // namespace util::meta
