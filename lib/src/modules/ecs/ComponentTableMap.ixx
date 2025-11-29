module;

#include <map>
#include <type_traits>
#include <utility>

#include "utility/contracts_macros.hpp"

export module ddge.modules.ecs:ComponentTableMap;

import ddge.utility.containers.Any;
import ddge.utility.containers.OptionalRef;

import :ArchetypeID;
import :component_c;
import :ComponentID;
import :ComponentTable;
import :ComponentTag;
import :decays_to_component_c;
import :ErasedComponentTable;
import :ErasedComponentTable.extensions;
import :RecordIndex;

class ComponentTableMap {
public:
    template <decays_to_component_c Component_T>
    auto insert(ArchetypeID archetype_id, Component_T&& component) -> RecordIndex;

    auto remove_component(
        ArchetypeID archetype_id,
        RecordIndex record_index,
        ComponentID component_id
    ) -> void;
    template <ddge::ecs::component_c Component_T>
    auto remove_component(ArchetypeID archetype_id, RecordIndex record_index)
        -> Component_T;

    auto move_component(
        ComponentID component_id,
        ArchetypeID archetype_id,
        RecordIndex record_index,
        ArchetypeID new_archetype_id
    ) -> RecordIndex;

    auto get_component_table(ComponentID component_id) -> ErasedComponentTable&;
    auto get_component_table(ComponentID component_id) const
        -> const ErasedComponentTable&;

    auto find_component_table(ComponentID component_id)
        -> ddge::util::OptionalRef<ErasedComponentTable>;
    auto find_component_table(ComponentID component_id) const
        -> ddge::util::OptionalRef<const ErasedComponentTable>;

private:
    std::map<ComponentID, ErasedComponentTable> m_map;
};

template <decays_to_component_c Component_T>
auto ComponentTableMap::insert(const ArchetypeID archetype_id, Component_T&& component)
    -> RecordIndex
{
    return ddge::util::any_cast<ComponentTable<std::decay_t<Component_T>>>(
               m_map
                   .try_emplace(
                       component_id_of<std::decay_t<Component_T>>(),
                       component_tag<std::decay_t<Component_T>>
                   )
                   .first->second
    )
        .insert(archetype_id, std::forward<Component_T>(component));
}

auto ComponentTableMap::remove_component(
    const ArchetypeID archetype_id,
    const RecordIndex record_index,
    const ComponentID component_id
) -> void
{
    const auto iterator = m_map.find(component_id);
    PRECOND(iterator != m_map.cend());

    iterator->second.remove_component(archetype_id, record_index);

    if (iterator->second.empty()) {
        m_map.erase(iterator);
    }
}

template <ddge::ecs::component_c Component_T>
auto ComponentTableMap::remove_component(
    const ArchetypeID archetype_id,
    const RecordIndex record_index
) -> Component_T
{
    const auto iterator = m_map.find(component_id_of<Component_T>());
    PRECOND(iterator != m_map.cend());

    Component_T result =
        ::remove_component<Component_T>(iterator->second, archetype_id, record_index);

    return result;
}

auto ComponentTableMap::move_component(
    const ComponentID component_id,
    const ArchetypeID archetype_id,
    const RecordIndex record_index,
    const ArchetypeID new_archetype_id
) -> RecordIndex
{
    const auto iterator{ m_map.find(component_id) };
    PRECOND(iterator != m_map.cend());

    const RecordIndex result =
        iterator->second.move_component(archetype_id, record_index, new_archetype_id);

    return result;
}

auto ComponentTableMap::get_component_table(const ComponentID component_id)
    -> ErasedComponentTable&
{
    const auto component_containers_iter{ m_map.find(component_id) };
    PRECOND(component_containers_iter != m_map.cend());

    return component_containers_iter->second;
}

auto ComponentTableMap::get_component_table(const ComponentID component_id) const
    -> const ErasedComponentTable&
{
    return const_cast<ComponentTableMap&>(*this).get_component_table(component_id);
}

auto ComponentTableMap::find_component_table(const ComponentID component_id)
    -> ddge::util::OptionalRef<ErasedComponentTable>
{
    const auto iterator{ m_map.find(component_id) };
    if (iterator == m_map.cend()) {
        return std::nullopt;
    }

    return iterator->second;
}

auto ComponentTableMap::find_component_table(const ComponentID component_id) const
    -> ddge::util::OptionalRef<const ErasedComponentTable>
{
    return const_cast<ComponentTableMap&>(*this).find_component_table(component_id);
}
