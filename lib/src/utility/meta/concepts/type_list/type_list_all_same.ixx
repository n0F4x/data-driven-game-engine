export module ddge.utility.meta.concepts.type_list.type_list_all_same;

import ddge.utility.meta.concepts.type_list.type_list;
import ddge.utility.meta.type_traits.type_list.type_list_all_same;

namespace ddge::util::meta {

export template <typename TypeList_T>
concept type_list_all_same_c = type_list_c<TypeList_T>
                            && type_list_all_same_v<TypeList_T>;

}   // namespace ddge::util::meta
