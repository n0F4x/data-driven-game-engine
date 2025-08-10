export module ddge.modules.ecs:query.queryable_component_c;

import :component_c;

namespace ddge::ecs {

export template <typename T>
concept queryable_component_c = component_c<T>;

}   // namespace ddge::ecs
