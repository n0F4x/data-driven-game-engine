module;

#include <type_traits>

export module ddge.modules.ecs:query.component_query_filter_c;

import :query.queryable_component_c;
import :query.query_filter_tag_c;

template <typename T>
concept component_query_filter_c =
    ddge::ecs::queryable_component_c<std::remove_const_t<T>>
    || ddge::ecs::query_filter_tag_c<T>;
