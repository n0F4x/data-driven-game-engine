export module ddge.modules.ecs:ComponentTable.extensions;

import ddge.utility.containers.OptionalRef;

import :ArchetypeID;
import :component_c;
import :ComponentContainer;
import :ComponentTable;
import :RecordIndex;

template <ddge::ecs::component_c Component_T>
[[nodiscard]]
auto get_component(
    ComponentTable<Component_T>& it,
    ArchetypeID                  archetype_id,
    RecordIndex                  record_index
) -> Component_T&;

template <ddge::ecs::component_c Component_T>
[[nodiscard]]
auto get_component(
    const ComponentTable<Component_T>& it,
    ArchetypeID                        archetype_id,
    RecordIndex                        record_index
) -> const Component_T&;

template <ddge::ecs::component_c Component_T>
auto find_component(
    ComponentTable<Component_T>& it,
    ArchetypeID                  archetype_id,
    RecordIndex                  record_index
) -> ddge::util::OptionalRef<Component_T>;

template <ddge::ecs::component_c Component_T>
auto find_component(
    const ComponentTable<Component_T>& it,
    ArchetypeID                        archetype_id,
    RecordIndex                        record_index
) -> ddge::util::OptionalRef<const Component_T>;

template <ddge::ecs::component_c Component_T>
auto get_component(
    ComponentTable<Component_T>& it,
    ArchetypeID                  archetype_id,
    RecordIndex                  record_index
) -> Component_T&
{
    return it.get_component_container(archetype_id).get(record_index);
}

template <ddge::ecs::component_c Component_T>
auto get_component(
    const ComponentTable<Component_T>& it,
    ArchetypeID                        archetype_id,
    RecordIndex                        record_index
) -> const Component_T&
{
    return get_component(
        const_cast<ComponentTable<Component_T>&>(it), archetype_id, record_index
    );
}

template <ddge::ecs::component_c Component_T>
auto find_component(
    ComponentTable<Component_T>& it,
    ArchetypeID                  archetype_id,
    RecordIndex                  record_index
) -> ddge::util::OptionalRef<Component_T>
{
    return it.find_component_container(archetype_id)
        .transform(
            [record_index](
                ComponentContainer<Component_T>& component_container
            ) -> decltype(auto) { return component_container.get(record_index); }
        );
}

template <ddge::ecs::component_c Component_T>
auto find_component(
    const ComponentTable<Component_T>& it,
    ArchetypeID                        archetype_id,
    RecordIndex                        record_index
) -> ddge::util::OptionalRef<const Component_T>
{
    return find_component(
        const_cast<ComponentTable<Component_T>&>(it), archetype_id, record_index
    );
}
