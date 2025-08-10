module;

#include <cstdint>

export module ddge.modules.ecs:ComponentID;

import ddge.utility.meta.reflection.hash;
import ddge.utility.Strong;

import :component_c;

struct ComponentID : ddge::util::Strong<uint_least32_t, ComponentID> {
    using Strong::Strong;
};

template <ddge::ecs::component_c Component_T>
struct ComponentIDOfClosure {
    constexpr static ComponentID value{ ddge::util::meta::hash<Component_T>() };

    consteval static auto operator()() -> ComponentID
    {
        return value;
    }
};

template <ddge::ecs::component_c Component_T>
inline constexpr ComponentIDOfClosure<Component_T> component_id_of;
