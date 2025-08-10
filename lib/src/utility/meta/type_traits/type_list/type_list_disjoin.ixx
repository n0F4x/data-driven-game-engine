export module ddge.utility.meta.type_traits.type_list.type_list_disjoin;

namespace ddge::util::meta {

export template <typename TypeList_T>
struct type_list_disjoin;

template <template <typename...> typename TypeList_T, typename... Ts>
struct type_list_disjoin<TypeList_T<Ts...>> {
    using type = TypeList_T<TypeList_T<Ts>...>;
};

export template <typename TypeList_T>
using type_list_disjoin_t = typename type_list_disjoin<TypeList_T>::type;

}   // namespace ddge::util::meta
