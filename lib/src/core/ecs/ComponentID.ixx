module;

#include <cstdint>

export module core.ecs:ComponentID;

import utility.meta.reflection.hash;
import utility.Strong;

import :component_c;

using ComponentID = ::util::Strong<uint_least32_t>;

template <core::ecs::component_c Component_T>
struct ComponentIDOfAdaptorClosure {
    constexpr static ComponentID value{ util::meta::hash<Component_T> };

    consteval static auto operator()() -> ComponentID
    {
        return value;
    }
};

template <core::ecs::component_c Component_T>
constexpr inline ComponentIDOfAdaptorClosure<Component_T> component_id_of;
