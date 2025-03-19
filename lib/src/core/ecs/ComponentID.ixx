module;

#include <cstdint>

export module core.ecs:ComponentID;

import utility.meta.reflection.type_id;
import utility.Strong;

import :component_c;

using ComponentID = ::util::Strong<uint_least32_t>;

template <core::ecs::component_c Component_T>
constexpr ComponentID component_id{ util::meta::id_v<Component_T> };
