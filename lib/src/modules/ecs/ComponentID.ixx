module;

#include <cstdint>

export module modules.ecs:ComponentID;

import utility.meta.reflection.hash;
import utility.Strong;

import :component_c;

struct ComponentID : ::util::Strong<uint_least32_t, ComponentID> {
    using Strong::Strong;
};

template <modules::ecs::component_c Component_T>
struct ComponentIDOfClosure {
    constexpr static ComponentID value{ util::meta::hash<Component_T>() };

    consteval static auto operator()() -> ComponentID
    {
        return value;
    }
};

template <modules::ecs::component_c Component_T>
inline constexpr ComponentIDOfClosure<Component_T> component_id_of;
