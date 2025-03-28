module;

#include <type_traits>

export module core.ecs:query.ToComponent;

import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.underlying;

import :query.component_query_parameter_c;
import :query.queryable_component_c;
import :query.query_parameter_tags.fwd;

template <component_query_parameter_c T>
struct ToComponent;

template <component_query_parameter_c T>
    requires core::ecs::queryable_component_c<std::remove_const_t<T>>
struct ToComponent<T> : std::type_identity<std::remove_const_t<T>> {};

template <component_query_parameter_c T>
    requires util::meta::specialization_of_c<T, core::ecs::With>
struct ToComponent<T>
    : std::type_identity<std::remove_const_t<util::meta::underlying_t<T>>> {};

template <component_query_parameter_c T>
    requires util::meta::specialization_of_c<T, core::ecs::Without>
struct ToComponent<T>
    : std::type_identity<std::remove_const_t<util::meta::underlying_t<T>>> {};

template <component_query_parameter_c T>
    requires util::meta::specialization_of_c<T, core::ecs::Optional>
struct ToComponent<T>
    : std::type_identity<
          std::remove_const_t<util::meta::underlying_t<std::remove_const_t<T>>>> {};
