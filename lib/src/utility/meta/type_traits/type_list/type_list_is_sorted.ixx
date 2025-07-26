module;

#include <type_traits>

export module utility.meta.type_traits.type_list.type_list_is_sorted;

import utility.meta.concepts.type_list.type_list;
import utility.meta.type_traits.type_list.type_list_sort;

namespace util::meta {

export template <type_list_c TypeList_T, template <typename> typename Hash_T>
struct type_list_is_sorted {
    constexpr static bool value =
        std::is_same_v<TypeList_T, type_list_sort_t<TypeList_T, Hash_T>>;
};

export template <type_list_c TypeList_T, template <typename> typename Hash_T>
inline constexpr bool type_list_is_sorted_v =
    type_list_is_sorted<TypeList_T, Hash_T>::value;

}   // namespace util::meta
