module;

#include <concepts>

export module core.ecs:query.query_parameter_tag_c;

import :query.QueryParameterTagBase;

namespace core::ecs {

export template <typename T>
concept query_parameter_tag_c = std::derived_from<T, QueryParameterTagBase>;

}   // namespace core::ecs
