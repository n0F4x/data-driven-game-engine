export module utility.meta.concepts.type_list.type_list;

import utility.meta.type_traits.type_list.is_type_list;

namespace util::meta {

export template <typename T>
concept type_list_c = is_type_list_v<T>;

}   // namespace util::meta
