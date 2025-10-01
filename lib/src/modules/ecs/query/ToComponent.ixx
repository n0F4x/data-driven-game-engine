module;

#include <type_traits>

export module ddge.modules.ecs:query.ToComponent;

import ddge.utility.meta.concepts.specialization_of;
import ddge.utility.meta.type_traits.underlying;

import :query.component_query_filter_c;
import :query.queryable_component_c;
import :query.query_filter_tags;

template <component_query_filter_c T>
struct ToComponent;

template <component_query_filter_c T>
    requires ddge::ecs::queryable_component_c<std::remove_const_t<T>>
struct ToComponent<T> : std::type_identity<T> {};

template <component_query_filter_c T>
    requires ddge::util::meta::specialization_of_c<T, ddge::ecs::With>
struct ToComponent<T> : std::type_identity<ddge::util::meta::underlying_t<T>> {};

template <component_query_filter_c T>
    requires ddge::util::meta::specialization_of_c<T, ddge::ecs::Without>
struct ToComponent<T> : std::type_identity<ddge::util::meta::underlying_t<T>> {};

template <component_query_filter_c T>
    requires ddge::util::meta::specialization_of_c<T, ddge::ecs::Optional>
struct ToComponent<T> : std::type_identity<ddge::util::meta::underlying_t<T>> {};
