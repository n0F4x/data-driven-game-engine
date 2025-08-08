module;

#include <concepts>

export module modules.ecs:query.query_parameter_tag_c;

import :query.QueryParameterTagBase;

namespace modules::ecs {

export template <typename T>
concept query_parameter_tag_c = std::derived_from<T, QueryParameterTagBase>;

}   // namespace modules::ecs
