module;

#include <utility>

export module ddge.modules.ecs:ErasedComponentTable.extensions;

import ddge.utility.containers.Any;
import ddge.utility.containers.OptionalRef;

import :ArchetypeID;
import :component_c;
import :ComponentTable.extensions;
import :decays_to_component_c;
import :ErasedComponentTable;
import :RecordIndex;

template <decays_to_component_c Component_T>
auto insert(ErasedComponentTable& it, ArchetypeID archetype_id, Component_T&& component)
    -> RecordIndex;

template <ddge::ecs::component_c Component_T>
auto remove_component(
    ErasedComponentTable& it,
    ArchetypeID           archetype_id,
    RecordIndex           record_index
) -> Component_T;

template <ddge::ecs::component_c Component_T>
[[nodiscard]]
auto get_component(
    ErasedComponentTable& it,
    ArchetypeID           archetype_id,
    RecordIndex           record_index
) -> Component_T&;

template <ddge::ecs::component_c Component_T>
[[nodiscard]]
auto get_component(
    const ErasedComponentTable& it,
    ArchetypeID                 archetype_id,
    RecordIndex                 record_index
) -> const Component_T&;

template <ddge::ecs::component_c Component_T>
auto find_component(
    ErasedComponentTable& it,
    ArchetypeID           archetype_id,
    RecordIndex           record_index
) -> ddge::util::OptionalRef<Component_T>;

template <ddge::ecs::component_c Component_T>
auto find_component(
    const ErasedComponentTable& it,
    ArchetypeID                 archetype_id,
    RecordIndex                 record_index
) -> ddge::util::OptionalRef<const Component_T>;

template <decays_to_component_c Component_T>
auto insert(
    ErasedComponentTable& it,
    const ArchetypeID     archetype_id,
    Component_T&&         component
) -> RecordIndex
{
    return ddge::util::any_cast<ComponentTable<Component_T>>(it).insert(
        archetype_id, std::forward<Component_T>(component)
    );
}

template <ddge::ecs::component_c Component_T>
auto remove_component(
    ErasedComponentTable& it,
    ArchetypeID           archetype_id,
    RecordIndex           record_index
) -> Component_T
{
    return ddge::util::any_cast<ComponentTable<Component_T>>(it).remove_component(
        archetype_id, record_index
    );
}

template <ddge::ecs::component_c Component_T>
auto get_component(
    ErasedComponentTable& it,
    const ArchetypeID     archetype_id,
    const RecordIndex     record_index
) -> Component_T&
{
    return get_component(
        ddge::util::any_cast<ComponentTable<Component_T>>(it), archetype_id, record_index
    );
}

template <ddge::ecs::component_c Component_T>
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

template <ddge::ecs::component_c Component_T>
auto find_component(
    ErasedComponentTable& it,
    const ArchetypeID     archetype_id,
    const RecordIndex     record_index
) -> ddge::util::OptionalRef<Component_T>
{
    return find_component(
        ddge::util::any_cast<ComponentTable<Component_T>>(it), archetype_id, record_index
    );
}

template <ddge::ecs::component_c Component_T>
auto find_component(
    const ErasedComponentTable& it,
    const ArchetypeID           archetype_id,
    const RecordIndex           record_index
) -> ddge::util::OptionalRef<const Component_T>
{
    return find_component<Component_T>(
        const_cast<ErasedComponentTable&>(it), archetype_id, record_index
    );
}
