export module utility.meta.concepts.type_list.type_list_all_same;

import utility.meta.concepts.type_list.type_list;
import utility.meta.type_traits.type_list.type_list_all_same;

namespace util::meta {

export template <typename TypeList_T>
concept type_list_all_same_c = type_list_c<TypeList_T>
                            && type_list_all_same_v<TypeList_T>;

}   // namespace util::meta
