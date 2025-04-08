export module core.ecs:ComponentTable.extensions;

import utility.containers.OptionalRef;

import :ArchetypeID;
import :component_c;
import :ComponentContainer;
import :ComponentTable;
import :RecordIndex;

inline namespace extensions {

template <core::ecs::component_c Component_T>
[[nodiscard]]
auto get_component(
    ComponentTable<Component_T>& it,
    ArchetypeID                  archetype_id,
    RecordIndex                  record_index
) -> Component_T&;

template <core::ecs::component_c Component_T>
[[nodiscard]]
auto get_component(
    const ComponentTable<Component_T>& it,
    ArchetypeID                        archetype_id,
    RecordIndex                        record_index
) -> const Component_T&;

template <core::ecs::component_c Component_T>
auto find_component(
    ComponentTable<Component_T>& it,
    ArchetypeID                  archetype_id,
    RecordIndex                  record_index
) -> util::OptionalRef<Component_T>;

template <core::ecs::component_c Component_T>
auto find_component(
    const ComponentTable<Component_T>& it,
    ArchetypeID                        archetype_id,
    RecordIndex                        record_index
) -> util::OptionalRef<const Component_T>;

}   // namespace extensions

template <core::ecs::component_c Component_T>
auto extensions::get_component(
    ComponentTable<Component_T>& it,
    ArchetypeID                  archetype_id,
    RecordIndex                  record_index
) -> Component_T&
{
    return it.get_component_container(archetype_id).get(record_index);
}

template <core::ecs::component_c Component_T>
auto extensions::get_component(
    const ComponentTable<Component_T>& it,
    ArchetypeID                        archetype_id,
    RecordIndex                        record_index
) -> const Component_T&
{
    return get_component(
        const_cast<ComponentTable<Component_T>&>(it), archetype_id, record_index
    );
}

template <core::ecs::component_c Component_T>
auto extensions::find_component(
    ComponentTable<Component_T>& it,
    ArchetypeID                  archetype_id,
    RecordIndex                  record_index
) -> util::OptionalRef<Component_T>
{
    return it.find_component_container(archetype_id)
        .transform(
            [record_index](
                ComponentContainer<Component_T>& component_container
            ) -> decltype(auto) { return component_container.get(record_index); }
        );
}

template <core::ecs::component_c Component_T>
auto extensions::find_component(
    const ComponentTable<Component_T>& it,
    ArchetypeID                        archetype_id,
    RecordIndex                        record_index
) -> util::OptionalRef<const Component_T>
{
    return find_component(
        const_cast<ComponentTable<Component_T>&>(it), archetype_id, record_index
    );
}
