module;

#include <type_traits>

export module ddge.ecs:query.query_filter_components_are_all_different_c;

import ddge.util.meta.concepts.type_list.type_list_all_different;
import ddge.util.meta.type_traits.type_list.type_list_filter;
import ddge.util.meta.type_traits.type_list.type_list_transform;
import ddge.util.meta.type_traits.underlying;
import ddge.util.TypeList;

import :query.component_query_filter_c;
import :query.query_filter_tag_c;
import :query.ToComponent;

template <typename T>
struct IsComponentFilter : std::bool_constant<component_query_filter_c<T>> {};

template <typename... QueryFilters_T>
concept query_filter_components_are_all_different_c =
    ddge::util::meta::type_list_all_different_c<ddge::util::meta::type_list_transform_t<
        ddge::util::meta::
            type_list_filter_t<ddge::util::TypeList<QueryFilters_T...>, IsComponentFilter>,
        ToComponent>>;
