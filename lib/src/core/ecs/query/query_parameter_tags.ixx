module;

#include <type_traits>

export module core.ecs:query.query_parameter_tags;

import utility.meta.concepts.specialization_of;

import :component_c;

namespace core::ecs {

export template <component_c>
struct With;

export template <component_c>
struct Without;

export template <typename T>
    requires component_c<std::remove_const_t<T>>
struct Optional;

export template <typename T>
concept query_parameter_tag_c = util::meta::specialization_of_c<T, With>
                             || util::meta::specialization_of_c<T, Without>
                             || util::meta::specialization_of_c<T, Optional>;

}   // namespace core::ecs
