module;

export module ddge.util.meta.concepts.specialization_of;

import ddge.util.meta.type_traits.is_specialization_of;

namespace ddge::util::meta {

export template <typename T, template <typename...> typename TypeList_T>
concept specialization_of_c = is_specialization_of_v<T, TypeList_T>;

}   // namespace ddge::util::meta
