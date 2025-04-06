module;

#include <type_traits>
#include <unordered_map>
#include <utility>

#include "utility/contracts.hpp"

export module core.ecs:ComponentTable;

import utility.meta.type_traits.const_like;
import utility.containers.OptionalRef;

import :ArchetypeID;
import :component_c;
import :ComponentContainer;
import :ComponentTag;
import :decays_to_component_c;
import :ErasedComponentContainer;
import :RecordIndex;

class ComponentTable {
    using Underlying = std::unordered_map<ArchetypeID, ErasedComponentContainer>;

public:
    template <decays_to_component_c Component_T>
    auto insert(ArchetypeID archetype_id, Component_T&& component) -> RecordIndex;

    auto remove_component(ArchetypeID archetype, RecordIndex record_index) -> void;
    template <core::ecs::component_c Component_T>
    auto remove_component(ArchetypeID archetype_id, RecordIndex record_index)
        -> Component_T;

    auto move_component(
        ArchetypeID archetype_id,
        RecordIndex record_index,
        ArchetypeID new_archetype_id
    ) -> RecordIndex;

    template <core::ecs::component_c Component_T, typename Self_T>
    auto get_component_container(this Self_T&& self, ArchetypeID archetype_id)
        -> util::meta::
            const_like_t<ComponentContainer<Component_T>, std::remove_reference_t<Self_T>>&;

    template <core::ecs::component_c Component_T, typename Self_T>
    auto find_component_container(this Self_T&& self, ArchetypeID archetype_id)
        -> util::OptionalRef<util::meta::const_like_t<
            ComponentContainer<Component_T>,
            std::remove_reference_t<Self_T>>>;

    [[nodiscard]]
    auto empty() const noexcept -> bool;
    [[nodiscard]]
    auto size() const noexcept -> size_t;

    [[nodiscard]]
    auto underlying() noexcept -> Underlying&;
    [[nodiscard]]
    auto underlying() const noexcept -> const Underlying&;

private:
    Underlying m_map;
};

template <core::ecs::component_c Component_T>
constexpr auto ErasedComponentContainerTraits<Component_T>::move_out_to(
    ErasedComponentContainer& self,
    const RecordIndex         record_index,
    ComponentTable&           component_table,
    const ArchetypeID         archetype_id
) -> RecordIndex
{
    PRECOND(archetype_id->contains_all_of_components<Component_T>());

    return component_table.insert(archetype_id, remove_impl(self, record_index));
}

template <decays_to_component_c Component_T>
auto ComponentTable::insert(const ArchetypeID archetype_id, Component_T&& component)
    -> RecordIndex
{
    using ComponentContainer = ComponentContainer<std::decay_t<Component_T>>;

    return m_map.try_emplace(archetype_id, component_tag<std::decay_t<Component_T>>)
        .first->second.template get<ComponentContainer>()
        .insert(std::forward<Component_T>(component));
}

template <core::ecs::component_c Component_T>
auto ComponentTable::remove_component(
    const ArchetypeID archetype_id,
    const RecordIndex record_index
) -> Component_T
{
    const auto iterator = m_map.find(archetype_id);
    PRECOND(iterator != m_map.cend());

    Component_T result = iterator->second.remove<Component_T>(record_index);

    if (iterator->second.empty()) {
        m_map.erase(iterator);
    }

    return result;
}

auto ComponentTable::remove_component(
    const ArchetypeID archetype,
    const RecordIndex record_index
) -> void
{
    const auto iterator = m_map.find(archetype);
    PRECOND(iterator != m_map.cend());

    iterator->second.remove(record_index);

    if (iterator->second.empty()) {
        m_map.erase(iterator);
    }
}

auto ComponentTable::move_component(
    const ArchetypeID archetype_id,
    const RecordIndex record_index,
    const ArchetypeID new_archetype_id
) -> RecordIndex
{
    const auto iterator{ m_map.find(archetype_id) };
    PRECOND(iterator != m_map.cend());

    const RecordIndex new_record_index =
        iterator->second.move_out_to(record_index, *this, new_archetype_id);

    if (iterator->second.empty()) {
        m_map.erase(iterator);
    }

    return new_record_index;
}

template <core::ecs::component_c Component_T, typename Self_T>
auto ComponentTable::get_component_container(
    this Self_T&&     self,
    const ArchetypeID archetype_id
) -> util::meta::
    const_like_t<ComponentContainer<Component_T>, std::remove_reference_t<Self_T>>&
{
    const auto component_container_iter{ self.m_map.find(archetype_id) };
    PRECOND(component_container_iter != self.m_map.cend());

    return component_container_iter
        ->second   //
        .template get<ComponentContainer<Component_T>>();
}

template <core::ecs::component_c Component_T, typename Self_T>
auto ComponentTable::find_component_container(
    this Self_T&&     self,
    const ArchetypeID archetype_id
)
    -> util::OptionalRef<
        util::meta::
            const_like_t<ComponentContainer<Component_T>, std::remove_reference_t<Self_T>>>
{
    const auto iterator{ self.m_map.find(archetype_id) };
    if (iterator == self.m_map.cend()) {
        return std::nullopt;
    }

    return iterator->second.template get<ComponentContainer<Component_T>>();
}

auto ComponentTable::empty() const noexcept -> bool
{
    return m_map.empty();
}

auto ComponentTable::size() const noexcept -> size_t
{
    return m_map.size();
}

auto ComponentTable::underlying() noexcept -> Underlying&
{
    return m_map;
}

auto ComponentTable::underlying() const noexcept -> const Underlying&
{
    return m_map;
}
