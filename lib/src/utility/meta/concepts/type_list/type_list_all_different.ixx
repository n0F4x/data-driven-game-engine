export module utility.meta.concepts.type_list.type_list_all_different;

import utility.meta.concepts.type_list.type_list;
import utility.meta.concepts.all_different;

template <typename>
struct type_list_all_different_impl;

template <template <typename...> typename TypeList_T, typename... Ts>
struct type_list_all_different_impl<TypeList_T<Ts...>> {
    constexpr static bool value = util::meta::all_different_c<Ts...>;
};

namespace util::meta {

export template <typename TypeList_T>
concept type_list_all_different_c =
    type_list_c<TypeList_T> && ::type_list_all_different_impl<TypeList_T>::value;

}   // namespace util::meta
