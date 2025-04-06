module;

#include <map>
#include <type_traits>
#include <utility>

#include "utility/contracts.hpp"

export module core.ecs:ComponentTableMap;

import utility.containers.OptionalRef;
import utility.meta.type_traits.const_like;

import :ArchetypeID;
import :component_c;
import :ComponentID;
import :ComponentTable;
import :decays_to_component_c;
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
    template <core::ecs::component_c Component_T>
    auto remove_component(ArchetypeID archetype_id, RecordIndex record_index)
        -> Component_T;

    auto move_component(
        ComponentID component_id,
        ArchetypeID archetype_id,
        RecordIndex record_index,
        ArchetypeID new_archetype_id
    ) -> RecordIndex;

    template <core::ecs::component_c Component_T, typename Self_T>
    auto get_component_table(this Self_T&& self)
        -> util::meta::const_like_t<ComponentTable, std::remove_reference_t<Self_T>>&;

    template <core::ecs::component_c Component_T, typename Self_T>
    auto find_component_table(this Self_T&& self) -> util::OptionalRef<
        util::meta::const_like_t<ComponentTable, std::remove_reference_t<Self_T>>>;

private:
    std::map<ComponentID, ComponentTable> m_map;
};

template <decays_to_component_c Component_T>
auto ComponentTableMap::insert(const ArchetypeID archetype_id, Component_T&& component)
    -> RecordIndex
{
    return m_map[::component_id_of<std::decay_t<Component_T>>()].insert(
        archetype_id, std::forward<Component_T>(component)
    );
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

template <core::ecs::component_c Component_T>
auto ComponentTableMap::remove_component(
    const ArchetypeID archetype_id,
    const RecordIndex record_index
) -> Component_T
{
    const auto iterator = m_map.find(component_id_of<Component_T>());
    PRECOND(iterator != m_map.cend());

    Component_T result = iterator->second.template remove_component<Component_T>(
        archetype_id, record_index
    );

    if (iterator->second.empty()) {
        m_map.erase(iterator);
    }

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

    if (iterator->second.empty()) {
        m_map.erase(iterator);
    }

    return result;
}

template <core::ecs::component_c Component_T, typename Self_T>
auto ComponentTableMap::get_component_table(this Self_T&& self)
    -> util::meta::const_like_t<ComponentTable, std::remove_reference_t<Self_T>>&
{
    const auto component_containers_iter{
        self.m_map.find(component_id_of<Component_T>())
    };
    PRECOND(component_containers_iter != self.m_map.cend());

    return component_containers_iter->second;
}

template <core::ecs::component_c Component_T, typename Self_T>
auto ComponentTableMap::find_component_table(this Self_T&& self) -> util::
    OptionalRef<util::meta::const_like_t<ComponentTable, std::remove_reference_t<Self_T>>>
{
    const auto iterator{ self.m_map.find(component_id_of<Component_T>()) };
    if (iterator == self.m_map.cend()) {
        return std::nullopt;
    }

    return iterator->second;
}
