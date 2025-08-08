module;

#include <type_traits>

export module modules.ecs:decays_to_component_c;

import :component_c;

template <typename T>
concept decays_to_component_c = modules::ecs::component_c<std::decay_t<T>>;
