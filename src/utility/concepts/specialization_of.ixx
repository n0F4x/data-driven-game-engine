module;

export module utility.concepts.specialization_of;

import utility.type_traits.is_specialization_of;

namespace util::meta {

export template <typename SpecializationT, template <typename...> typename T>
concept specialization_of_c = is_specialization_of_v<SpecializationT, T>;

}   // namespace util::meta
