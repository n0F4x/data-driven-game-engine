module;

#include <type_traits>

export module core.ecs:specialization_of_registry_c;

import :fwd;

namespace core::ecs {

template <typename>
struct is_specialization_of_registry : std::false_type {};

template <auto tag_T>
struct is_specialization_of_registry<Registry<tag_T>> : std::true_type {};

export template <typename T>
concept specialization_of_registry_c = is_specialization_of_registry<T>::value;

}   // namespace core::ecs
