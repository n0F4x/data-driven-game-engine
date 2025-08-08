module;

#include <type_traits>

export module modules.ecs:query.ToComponent;

import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.underlying;

import :query.component_query_parameter_c;
import :query.queryable_component_c;
import :query.query_parameter_tags;

template <component_query_parameter_c T>
struct ToComponent;

template <component_query_parameter_c T>
    requires modules::ecs::queryable_component_c<std::remove_const_t<T>>
struct ToComponent<T> : std::type_identity<T> {};

template <component_query_parameter_c T>
    requires util::meta::specialization_of_c<T, modules::ecs::With>
struct ToComponent<T> : std::type_identity<util::meta::underlying_t<T>> {};

template <component_query_parameter_c T>
    requires util::meta::specialization_of_c<T, modules::ecs::Without>
struct ToComponent<T> : std::type_identity<util::meta::underlying_t<T>> {};

template <component_query_parameter_c T>
    requires util::meta::specialization_of_c<T, modules::ecs::Optional>
struct ToComponent<T> : std::type_identity<util::meta::underlying_t<T>> {};
