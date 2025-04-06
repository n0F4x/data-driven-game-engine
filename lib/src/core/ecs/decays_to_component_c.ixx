module;

#include <type_traits>

export module core.ecs:decays_to_component_c;

import :component_c;

template <typename T>
concept decays_to_component_c = core::ecs::component_c<std::decay_t<T>>;
