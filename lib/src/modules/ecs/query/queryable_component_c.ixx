module;

#include <type_traits>

export module ddge.modules.ecs:query.queryable_component_c;

import :component_c;

namespace ddge::ecs {

export template <typename T>
concept queryable_component_c = component_c<T> && !std::is_empty_v<T>;

}   // namespace ddge::ecs
