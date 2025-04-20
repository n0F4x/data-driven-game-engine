module;

#include <utility>

export module core.ecs:ErasedComponentTable.extensions;

import utility.containers.Any;
import utility.containers.OptionalRef;

import :ArchetypeID;
import :component_c;
import :ComponentTable.extensions;
import :decays_to_component_c;
import :ErasedComponentTable;
import :RecordIndex;

template <decays_to_component_c Component_T>
auto insert(ErasedComponentTable& it, ArchetypeID archetype_id, Component_T&& component)
    -> RecordIndex;

template <core::ecs::component_c Component_T>
auto remove_component(
    ErasedComponentTable& it,
    ArchetypeID           archetype_id,
    RecordIndex           record_index
) -> Component_T;

template <core::ecs::component_c Component_T>
[[nodiscard]]
auto get_component(
    ErasedComponentTable& it,
    ArchetypeID           archetype_id,
    RecordIndex           record_index
) -> Component_T&;

template <core::ecs::component_c Component_T>
[[nodiscard]]
auto get_component(
    const ErasedComponentTable& it,
    ArchetypeID                 archetype_id,
    RecordIndex                 record_index
) -> const Component_T&;

template <core::ecs::component_c Component_T>
auto find_component(
    ErasedComponentTable& it,
    ArchetypeID           archetype_id,
    RecordIndex           record_index
) -> util::OptionalRef<Component_T>;

template <core::ecs::component_c Component_T>
auto find_component(
    const ErasedComponentTable& it,
    ArchetypeID                 archetype_id,
    RecordIndex                 record_index
) -> util::OptionalRef<const Component_T>;

template <decays_to_component_c Component_T>
auto insert(
    ErasedComponentTable& it,
    const ArchetypeID     archetype_id,
    Component_T&&         component
) -> RecordIndex
{
    return util::any_cast<ComponentTable<Component_T>>(it).insert(
        archetype_id, std::forward<Component_T>(component)
    );
}

template <core::ecs::component_c Component_T>
auto remove_component(
    ErasedComponentTable& it,
    ArchetypeID           archetype_id,
    RecordIndex           record_index
) -> Component_T
{
    return util::any_cast<ComponentTable<Component_T>>(it).remove_component(
        archetype_id, record_index
    );
}

template <core::ecs::component_c Component_T>
auto get_component(
    ErasedComponentTable& it,
    const ArchetypeID     archetype_id,
    const RecordIndex     record_index
) -> Component_T&
{
    return get_component(
        util::any_cast<ComponentTable<Component_T>>(it), archetype_id, record_index
    );
}

template <core::ecs::component_c Component_T>
auto get_component(
    const ErasedComponentTable& it,
    const ArchetypeID           archetype_id,
    const RecordIndex           record_index
) -> const Component_T&
{
    return get_component<Component_T>(
        const_cast<ErasedComponentTable&>(it), archetype_id, record_index
    );
}

template <core::ecs::component_c Component_T>
auto find_component(
    ErasedComponentTable& it,
    const ArchetypeID     archetype_id,
    const RecordIndex     record_index
) -> util::OptionalRef<Component_T>
{
    return find_component(
        util::any_cast<ComponentTable<Component_T>>(it), archetype_id, record_index
    );
}

template <core::ecs::component_c Component_T>
auto find_component(
    const ErasedComponentTable& it,
    const ArchetypeID           archetype_id,
    const RecordIndex           record_index
) -> util::OptionalRef<const Component_T>
{
    return find_component<Component_T>(
        const_cast<ErasedComponentTable&>(it), archetype_id, record_index
    );
}
