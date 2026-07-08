export module ddge.utility.meta.type_traits.type_list.type_list_unique;

namespace ddge::util::meta {

export template <typename TypeList_T>
struct type_list_unique;

template <template <typename...> typename TypeList_T, typename... Ts>
struct type_list_unique<TypeList_T<Ts...>> {
    using type = TypeList_T<__builtin_dedup_pack<Ts...>...>;
};

export template <typename TypeList_T>
using type_list_unique_t = typename type_list_unique<TypeList_T>::type;

}   // namespace ddge::util::meta
