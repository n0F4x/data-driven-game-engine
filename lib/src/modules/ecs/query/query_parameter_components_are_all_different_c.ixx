module;

#include <tuple>
#include <type_traits>

export module modules.ecs:query.query_parameter_components_are_all_different_c;

import utility.meta.concepts.type_list.type_list_all_different;
import utility.meta.type_traits.type_list.type_list_filter;
import utility.meta.type_traits.type_list.type_list_transform;
import utility.meta.type_traits.underlying;

import :query.component_query_parameter_c;
import :query.query_parameter_tag_c;
import :query.ToComponent;

template <typename T>
struct IsComponentParameter : std::bool_constant<component_query_parameter_c<T>> {};

template <typename... QueryParameters_T>
concept query_parameter_components_are_all_different_c =
    util::meta::type_list_all_different_c<util::meta::type_list_transform_t<
        util::meta::type_list_filter_t<std::tuple<QueryParameters_T...>, IsComponentParameter>,
        ToComponent>>;
