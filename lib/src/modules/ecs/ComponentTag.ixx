export module ddge.modules.ecs:ComponentTag;

import :component_c;

template <ddge::ecs::component_c>
struct ComponentTag {};

template <ddge::ecs::component_c Component_T>
constexpr ComponentTag<Component_T> component_tag{};
