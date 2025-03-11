module;

export module utility.meta.concepts.specialization_of;

import utility.meta.type_traits.is_specialization_of;

namespace util::meta {

export template <typename T, template <typename...> typename TypeList_T>
concept specialization_of_c = is_specialization_of_v<T, TypeList_T>;

}   // namespace util::meta
