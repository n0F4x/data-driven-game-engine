module;

#include <concepts>

export module ddge.modules.ecs:query.query_parameter_tag_c;

import :query.QueryParameterTagBase;

namespace ddge::ecs {

export template <typename T>
concept query_parameter_tag_c = std::derived_from<T, QueryParameterTagBase>;

}   // namespace ddge::ecs
