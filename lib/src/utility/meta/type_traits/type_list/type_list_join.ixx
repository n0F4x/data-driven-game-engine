module;

export module utility.meta.type_traits.type_list.type_list_join;

import utility.meta.concepts.type_list.type_list;
import utility.meta.type_traits.type_list.type_list_concat;

namespace util::meta {

export template <typename TypeList_T>
struct type_list_join;

template <template <typename...> typename TypeList_T>
struct type_list_join<TypeList_T<>> {
    using type = TypeList_T<>;
};

template <template <typename...> typename TypeList_T, type_list_c InnerTypeList_T>
struct type_list_join<TypeList_T<InnerTypeList_T>> {
    using type = InnerTypeList_T;
};

template <template <typename...> typename TypeList_T, type_list_c... TypeLists_T>
    requires(sizeof...(TypeLists_T) >= 2)
struct type_list_join<TypeList_T<TypeLists_T...>> {
    using type = type_list_concat_t<TypeLists_T...>;
};

export template <typename TypeList_T>
using type_list_join_t = typename type_list_join<TypeList_T>::type;

}   // namespace util::meta
