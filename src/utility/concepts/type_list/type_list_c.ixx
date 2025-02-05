export module utility.concepts.type_list.type_list_c;

import utility.type_traits.type_list.is_type_list;

namespace util::meta {

export template <typename T>
concept type_list_c = is_type_list_v<T>;

}   // namespace util::meta
