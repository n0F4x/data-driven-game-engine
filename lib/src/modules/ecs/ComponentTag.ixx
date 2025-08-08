export module modules.ecs:ComponentTag;

import :component_c;

template <modules::ecs::component_c>
struct ComponentTag {};

template <modules::ecs::component_c Component_T>
constexpr ComponentTag<Component_T> component_tag{};
