module;

#include <vector>

export module core.ecs:ComponentContainer;

import :component_c;

template <core::ecs::component_c Component_T>
using ComponentContainer = std::vector<Component_T>;
