module;

#include <type_traits>

export module modules.ecs:query.component_query_parameter_c;

import :query.queryable_component_c;
import :query.query_parameter_tag_c;

template <typename T>
concept component_query_parameter_c =
    modules::ecs::queryable_component_c<std::remove_const_t<T>>
    || modules::ecs::query_parameter_tag_c<T>;
