module;

#include <cstdint>

export module ddge.ecs:ComponentID;

import ddge.util.meta.reflection.hash;
import ddge.util.containers.Strong;

import :component_c;

struct ComponentID : ddge::util::Strong<uint32_t, ComponentID> {
    using Strong::Strong;
};

template <ddge::ecs::component_c Component_T>
[[nodiscard]]
consteval auto component_id_of() -> ComponentID
{
    constexpr static ComponentID value{ ddge::util::meta::hash_u32<Component_T>() };
    return value;
}
