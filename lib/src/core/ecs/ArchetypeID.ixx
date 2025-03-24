module;

#include <cstdint>

export module core.ecs:ArchetypeID;

import utility.Strong;

import :ComponentID;
import :sorted_component_id_sequence;

// TODO: remove explicit []{} tag when Clang allows it
using ArchetypeID = util::Strong<uint_least32_t, [] {}>;

template <core::ecs::component_c... Components_T>
constexpr ArchetypeID archetype_id{
    util::meta::
        id_v<sorted_component_id_sequence_t<component_id<Components_T>.underlying()...>>
};
