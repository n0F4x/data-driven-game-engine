module;

#include <algorithm>
#include <span>
#include <unordered_map>
#include <utility>
#include <vector>

#include "utility/contracts_macros.hpp"

export module core.ecs:ComponentTable;

import utility.containers.OptionalRef;

import :ArchetypeID;
import :component_c;
import :ComponentContainer;
import :RecordIndex;

template <core::ecs::component_c Component_T>
class ComponentTable {
    using Underlying = std::unordered_map<ArchetypeID, ComponentContainer<Component_T>>;

public:
    template <util::meta::decays_to_c<Component_T> UComponent_T>
    auto insert(ArchetypeID archetype_id, UComponent_T&& component) -> RecordIndex;

    auto remove_component(ArchetypeID archetype_id, RecordIndex record_index)
        -> Component_T;

    auto move_component(
        ArchetypeID archetype_id,
        RecordIndex record_index,
        ArchetypeID new_archetype_id
    ) -> RecordIndex;

    [[nodiscard]]
    auto get_component_container(ArchetypeID archetype_id)
        -> ComponentContainer<Component_T>&;
    [[nodiscard]]
    auto get_component_container(ArchetypeID archetype_id) const
        -> const ComponentContainer<Component_T>&;

    auto find_component_container(ArchetypeID archetype_id)
        -> util::OptionalRef<ComponentContainer<Component_T>>;
    auto find_component_container(ArchetypeID archetype_id) const
        -> util::OptionalRef<const ComponentContainer<Component_T>>;

    [[nodiscard]]
    auto empty() const noexcept -> bool;
    [[nodiscard]]
    auto size() const noexcept -> size_t;

    [[nodiscard]]
    auto archetype_ids() const noexcept -> std::span<const ArchetypeID>;

private:
    Underlying               m_map;
    // TODO: use flat_map
    std::vector<ArchetypeID> m_archetype_ids;
};

template <core::ecs::component_c Component_T>
template <util::meta::decays_to_c<Component_T> UComponent_T>
auto ComponentTable<Component_T>::insert(
    const ArchetypeID archetype_id,
    UComponent_T&&    component
) -> RecordIndex
{
    if (!std::ranges::contains(m_archetype_ids, archetype_id)) {
        m_archetype_ids.push_back(archetype_id);
    }

    return m_map[archetype_id].insert(std::forward<UComponent_T>(component));
}

template <core::ecs::component_c Component_T>
auto ComponentTable<Component_T>::remove_component(
    const ArchetypeID archetype_id,
    const RecordIndex record_index
) -> Component_T
{
    const auto iterator = m_map.find(archetype_id);
    PRECOND(iterator != m_map.cend());

    Component_T result = iterator->second.remove(record_index);

    if (iterator->second.empty()) {
        (*std::ranges::find(m_archetype_ids, archetype_id)) = m_archetype_ids.back();
        m_archetype_ids.pop_back();

        m_map.erase(iterator);
    }

    return result;
}

template <core::ecs::component_c Component_T>
auto ComponentTable<Component_T>::move_component(
    const ArchetypeID archetype_id,
    const RecordIndex record_index,
    const ArchetypeID new_archetype_id
) -> RecordIndex
{
    PRECOND(new_archetype_id->contains_all_of_components<Component_T>());
    PRECOND(archetype_id != new_archetype_id);

    const auto iterator{ m_map.find(archetype_id) };
    PRECOND(iterator != m_map.cend());

    const RecordIndex new_record_index =
        insert(new_archetype_id, iterator->second.remove(record_index));

    if (iterator->second.empty()) {
        (*std::ranges::find(m_archetype_ids, archetype_id)) = m_archetype_ids.back();
        m_archetype_ids.pop_back();

        m_map.erase(iterator);
    }

    return new_record_index;
}

template <core::ecs::component_c Component_T>
auto ComponentTable<Component_T>::get_component_container(const ArchetypeID archetype_id)
    -> ComponentContainer<Component_T>&
{
    const auto component_container_iter{ m_map.find(archetype_id) };
    PRECOND(component_container_iter != m_map.cend());

    return component_container_iter->second;
}

template <core::ecs::component_c Component_T>
auto ComponentTable<Component_T>::get_component_container(
    const ArchetypeID archetype_id
) const -> const ComponentContainer<Component_T>&
{
    return const_cast<ComponentTable&>(*this).get_component_container(archetype_id);
}

template <core::ecs::component_c Component_T>
auto ComponentTable<Component_T>::find_component_container(const ArchetypeID archetype_id)
    -> util::OptionalRef<ComponentContainer<Component_T>>
{
    const auto iterator{ m_map.find(archetype_id) };
    if (iterator == m_map.cend()) {
        return std::nullopt;
    }

    return iterator->second;
}

template <core::ecs::component_c Component_T>
auto ComponentTable<Component_T>::find_component_container(
    const ArchetypeID archetype_id
) const -> util::OptionalRef<const ComponentContainer<Component_T>>
{
    return const_cast<ComponentTable&>(*this).find_component_container(archetype_id);
}

template <core::ecs::component_c Component_T>
auto ComponentTable<Component_T>::empty() const noexcept -> bool
{
    return m_map.empty();
}

template <core::ecs::component_c Component_T>
auto ComponentTable<Component_T>::size() const noexcept -> size_t
{
    return m_map.size();
}

template <core::ecs::component_c Component_T>
auto ComponentTable<Component_T>::archetype_ids() const noexcept
    -> std::span<const ArchetypeID>
{
    return m_archetype_ids;
}
