export module ddge.utility.meta.type_traits.type_list.type_list_front;

namespace ddge::util::meta {

export template <typename>
struct type_list_front;

template <template <typename...> typename TypeList_T, typename T, typename... Ts>
struct type_list_front<TypeList_T<T, Ts...>> {
    using type = T;
};

export template <typename TypeList_T>
using type_list_front_t = typename type_list_front<TypeList_T>::type;

}   // namespace ddge::util::meta
