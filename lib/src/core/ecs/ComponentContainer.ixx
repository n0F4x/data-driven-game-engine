module;

#include <optional>
#include <utility>
#include <vector>

#include "utility/contracts.hpp"

export module core.ecs:ComponentContainer;

import utility.meta.concepts.decays_to;

import :component_c;
import :RecordIndex;

template <core::ecs::component_c Component_T>
class ComponentContainer {
    using Underlying = std::vector<Component_T>;

public:
    using Iterator      = typename Underlying::iterator;
    using ConstIterator = typename Underlying::const_iterator;

    template <util::meta::decays_to_c<Component_T> UComponent_T>
    auto insert(UComponent_T&& component) -> RecordIndex;

    auto remove(RecordIndex record_index) -> Component_T;
    auto erase(RecordIndex record_index) -> std::optional<Component_T>;

    [[nodiscard]]
    auto get(RecordIndex record_index) -> Component_T&;
    [[nodiscard]]
    auto get(RecordIndex record_index) const -> const Component_T&;

    [[nodiscard]]
    auto empty() const noexcept -> bool;
    [[nodiscard]]
    auto size() const noexcept -> size_t;

    [[nodiscard]]
    auto begin() -> Iterator;
    [[nodiscard]]
    auto begin() const -> ConstIterator;

    [[nodiscard]]
    auto end() -> Iterator;
    [[nodiscard]]
    auto end() const -> ConstIterator;

private:
    Underlying m_vector;
};

template <core::ecs::component_c Component_T>
template <util::meta::decays_to_c<Component_T> UComponent_T>
auto ComponentContainer<Component_T>::insert(UComponent_T&& component) -> RecordIndex
{
    m_vector.push_back(std::forward<UComponent_T>(component));

    return RecordIndex{ static_cast<RecordIndex::Underlying>(m_vector.size() - 1uz) };
}

template <core::ecs::component_c Component_T>
auto ComponentContainer<Component_T>::remove(const RecordIndex record_index)
    -> Component_T
{
    PRECOND(record_index.underlying() < m_vector.size());

    Component_T result{ std::move(m_vector[record_index.underlying()]) };

    m_vector[record_index.underlying()] = std::move(m_vector.back());
    m_vector.pop_back();

    return result;
}

template <core::ecs::component_c Component_T>
auto ComponentContainer<Component_T>::erase(const RecordIndex record_index)
    -> std::optional<Component_T>
{
    if (record_index.underlying() >= m_vector.size()) {
        return std::nullopt;
    }

    std::optional<Component_T> result{ std::move(m_vector[record_index.underlying()]) };

    m_vector[record_index.underlying()] = std::move(m_vector.back());
    m_vector.pop_back();

    return result;
}

template <core::ecs::component_c Component_T>
auto ComponentContainer<Component_T>::get(const RecordIndex record_index) -> Component_T&
{
    return m_vector[record_index.underlying()];
}

template <core::ecs::component_c Component_T>
auto ComponentContainer<Component_T>::get(const RecordIndex record_index) const
    -> const Component_T&
{
    return const_cast<ComponentContainer&>(*this).get(record_index);
}

template <core::ecs::component_c Component_T>
auto ComponentContainer<Component_T>::empty() const noexcept -> bool
{
    return m_vector.empty();
}

template <core::ecs::component_c Component_T>
auto ComponentContainer<Component_T>::size() const noexcept -> size_t
{
    return m_vector.size();
}

template <core::ecs::component_c Component_T>
auto ComponentContainer<Component_T>::begin() -> Iterator
{
    return m_vector.begin();
}

template <core::ecs::component_c Component_T>
auto ComponentContainer<Component_T>::begin() const -> ConstIterator
{
    return m_vector.begin();
}

template <core::ecs::component_c Component_T>
auto ComponentContainer<Component_T>::end() -> Iterator
{
    return m_vector.end();
}

template <core::ecs::component_c Component_T>
auto ComponentContainer<Component_T>::end() const -> ConstIterator
{
    return m_vector.end();
}
