module;

#include <concepts>

export module ddge.modules.ecs:query.query_filter_tag_c;

import :query.QueryFilterTagBase;

namespace ddge::ecs {

export template <typename T>
concept query_filter_tag_c = std::derived_from<T, QueryFilterTagBase>;

}   // namespace ddge::ecs
