export module ddge.utility.meta.type_traits.type_list.type_list_single;

namespace ddge::util::meta {

export template <typename>
struct type_list_single;

template <template <typename...> typename TypeList_T, typename T>
struct type_list_single<TypeList_T<T>> {
    using type = T;
};

export template <typename TypeList_T>
using type_list_single_t = typename type_list_single<TypeList_T>::type;

}   // namespace ddge::util::meta
