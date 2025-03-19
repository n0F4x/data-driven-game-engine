export module core.ecs:ComponentTag;

import :component_c;

template <core::ecs::component_c>
struct ComponentTag {};

template <core::ecs::component_c Component_T>
constexpr ComponentTag<Component_T> component_tag{};
