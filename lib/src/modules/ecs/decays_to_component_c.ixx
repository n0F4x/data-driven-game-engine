module;

#include <type_traits>

export module ddge.modules.ecs:decays_to_component_c;

import :component_c;

template <typename T>
concept decays_to_component_c = ddge::ecs::component_c<std::decay_t<T>>;
