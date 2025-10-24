export module ddge.utility.meta.concepts.type_list.type_list_contains;

import ddge.utility.meta.concepts.type_list.type_list;
import ddge.utility.meta.type_traits.type_list.type_list_contains;

namespace ddge::util::meta {

export template <typename TypeList_T, typename T>
concept type_list_contains_c = type_list_c<TypeList_T>
                            && type_list_contains_v<TypeList_T, T>;

}   // namespace ddge::util::meta
