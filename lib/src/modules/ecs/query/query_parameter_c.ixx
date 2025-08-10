module;

#include <concepts>

export module ddge.modules.ecs:query.query_parameter_c;

import :ID;
import :query.component_query_parameter_c;

namespace ddge::ecs {

export template <typename T>
concept query_parameter_c = component_query_parameter_c<T> || std::same_as<T, ID>;

}   // namespace ddge::ecs
