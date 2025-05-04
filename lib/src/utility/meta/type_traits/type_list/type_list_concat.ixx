export module utility.meta.type_traits.type_list.type_list_concat;

namespace util::meta {

export template <typename...>
struct type_list_concat;

template <template <typename...> typename TypeList_T, typename... Ts>
struct type_list_concat<TypeList_T<Ts...>> {
    using type = TypeList_T<Ts...>;
};

template <template <typename...> typename TypeList_T, typename... Ts0, typename... Ts1>
struct type_list_concat<TypeList_T<Ts0...>, TypeList_T<Ts1...>> {
    using type = TypeList_T<Ts0..., Ts1...>;
};

template <typename TypeList0_T, typename TypeList1_T, typename TypeList2_T, typename... TypeLists_T>
struct type_list_concat<TypeList0_T, TypeList1_T, TypeList2_T, TypeLists_T...> {
    using type = typename type_list_concat<
        TypeList0_T,
        typename type_list_concat<TypeList1_T, TypeList2_T, TypeLists_T...>::type>::type;
};

export template <typename... TypeLists_T>
    requires(sizeof...(TypeLists_T) != 0)
using type_list_concat_t = typename type_list_concat<TypeLists_T...>::type;

}   // namespace util::meta
