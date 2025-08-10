export module ddge.utility.meta.concepts.type_list.type_list;

import ddge.utility.meta.type_traits.type_list.is_type_list;

namespace ddge::util::meta {

export template <typename T>
concept type_list_c = is_type_list_v<T>;

}   // namespace ddge::util::meta
