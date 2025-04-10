export module core.ecs:query.queryable_component_c;

import :component_c;

namespace core::ecs {

export template <typename T>
concept queryable_component_c = component_c<T>;

}   // namespace core::ecs
