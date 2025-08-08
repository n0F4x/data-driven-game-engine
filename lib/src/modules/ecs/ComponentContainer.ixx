module;

#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

#include "utility/contracts_macros.hpp"

export module modules.ecs:ComponentContainer;

import utility.contracts;
import utility.meta.concepts.decays_to;
import utility.meta.type_traits.maybe_const;

import :component_c;
import :RecordIndex;

template <modules::ecs::component_c Component_T>
class ComponentContainer {
    using Underlying = std::vector<Component_T>;

public:
    using Iterator      = typename Underlying::iterator;
    using ConstIterator = typename Underlying::const_iterator;

    template <util::meta::decays_to_c<Component_T> UComponent_T>
    constexpr auto insert(UComponent_T&& component) -> RecordIndex;

    constexpr auto remove(RecordIndex record_index) -> Component_T;
    constexpr auto erase(RecordIndex record_index) -> std::optional<Component_T>;

    [[nodiscard]]
    constexpr auto get(RecordIndex record_index) -> Component_T&;
    [[nodiscard]]
    constexpr auto get(RecordIndex record_index) const -> const Component_T&;

    [[nodiscard]]
    constexpr auto empty() const noexcept -> bool;
    [[nodiscard]]
    constexpr auto size() const noexcept -> std::size_t;

    [[nodiscard]]
    constexpr auto begin() -> Iterator;
    [[nodiscard]]
    constexpr auto begin() const -> ConstIterator;

    [[nodiscard]]
    constexpr auto end() -> Iterator;
    [[nodiscard]]
    constexpr auto end() const -> ConstIterator;

private:
    Underlying m_vector;
};

template <modules::ecs::component_c Component_T>
    requires(std::is_empty_v<Component_T>)
class ComponentContainer<Component_T> {
public:
    template <bool is_const_T>
    class Iterator;

    template <util::meta::decays_to_c<Component_T> UComponent_T>
    constexpr auto insert(UComponent_T&& component) -> RecordIndex;

    constexpr auto remove(RecordIndex record_index) -> Component_T;
    constexpr auto erase(RecordIndex record_index) -> std::optional<Component_T>;

    [[nodiscard]]
    constexpr auto get(RecordIndex record_index) -> Component_T&;
    [[nodiscard]]
    constexpr auto get(RecordIndex record_index) const -> const Component_T&;

    [[nodiscard]]
    constexpr auto empty() const noexcept -> bool;
    [[nodiscard]]
    constexpr auto size() const noexcept -> std::size_t;

    [[nodiscard]]
    constexpr auto begin() -> Iterator<false>;
    [[nodiscard]]
    constexpr auto begin() const -> Iterator<true>;

    [[nodiscard]]
    constexpr auto end() -> Iterator<false>;
    [[nodiscard]]
    constexpr auto end() const -> Iterator<true>;

private:
    std::optional<Component_T> m_optional;
    std::size_t                m_size{};
};

template <modules::ecs::component_c Component_T>
    requires(std::is_empty_v<Component_T>)
template <bool is_const_T>
class ComponentContainer<Component_T>::Iterator {
public:
    using difference_type = std::ptrdiff_t;
    using value_type      = Component_T;

    Iterator() = default;

    constexpr Iterator(
        util::meta::maybe_const_t<is_const_T, ComponentContainer>& container,
        const std::size_t                                          index
    )
        : m_base{ &container },
          m_current{ index }
    {}

    [[nodiscard]]
    auto operator==(const Iterator&) const -> bool = default;

    [[nodiscard]]
    constexpr auto operator==(const Iterator<!is_const_T>& other) const -> bool
    {
        return *this == std::bit_cast<Iterator>(other);
    };

    [[nodiscard]]
    constexpr auto operator*() const
        -> util::meta::maybe_const_t<is_const_T, Component_T>&
    {
        return *m_base->m_optional;
    }

    constexpr auto operator++() -> Iterator&
    {
        ++m_current;
        return *this;
    }

    constexpr auto operator++(int) -> Iterator
    {
        Iterator result{ *this };
        ++*this;
        return result;
    }

private:
    util::meta::maybe_const_t<is_const_T, ComponentContainer>* m_base{};
    std::size_t                                                m_current{};
};

template <modules::ecs::component_c Component_T>
template <util::meta::decays_to_c<Component_T> UComponent_T>
constexpr auto ComponentContainer<Component_T>::insert(UComponent_T&& component)
    -> RecordIndex
{
    m_vector.push_back(std::forward<UComponent_T>(component));

    return RecordIndex{ static_cast<RecordIndex::Underlying>(m_vector.size() - 1uz) };
}

template <modules::ecs::component_c Component_T>
constexpr auto ComponentContainer<Component_T>::remove(const RecordIndex record_index)
    -> Component_T
{
    PRECOND(record_index.underlying() < m_vector.size());

    Component_T result{ std::move(m_vector[record_index.underlying()]) };

    m_vector[record_index.underlying()] = std::move(m_vector.back());
    m_vector.pop_back();

    return result;
}

template <modules::ecs::component_c Component_T>
constexpr auto ComponentContainer<Component_T>::erase(const RecordIndex record_index)
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

template <modules::ecs::component_c Component_T>
constexpr auto ComponentContainer<Component_T>::get(const RecordIndex record_index)
    -> Component_T&
{
    return m_vector[record_index.underlying()];
}

template <modules::ecs::component_c Component_T>
constexpr auto ComponentContainer<Component_T>::get(const RecordIndex record_index) const
    -> const Component_T&
{
    return const_cast<ComponentContainer&>(*this).get(record_index);
}

template <modules::ecs::component_c Component_T>
constexpr auto ComponentContainer<Component_T>::empty() const noexcept -> bool
{
    return m_vector.empty();
}

template <modules::ecs::component_c Component_T>
constexpr auto ComponentContainer<Component_T>::size() const noexcept -> std::size_t
{
    return m_vector.size();
}

template <modules::ecs::component_c Component_T>
constexpr auto ComponentContainer<Component_T>::begin() -> Iterator
{
    return m_vector.begin();
}

template <modules::ecs::component_c Component_T>
constexpr auto ComponentContainer<Component_T>::begin() const -> ConstIterator
{
    return m_vector.begin();
}

template <modules::ecs::component_c Component_T>
constexpr auto ComponentContainer<Component_T>::end() -> Iterator
{
    return m_vector.end();
}

template <modules::ecs::component_c Component_T>
constexpr auto ComponentContainer<Component_T>::end() const -> ConstIterator
{
    return m_vector.end();
}

template <modules::ecs::component_c Component_T>
    requires(std::is_empty_v<Component_T>)
template <util::meta::decays_to_c<Component_T> UComponent_T>
constexpr auto ComponentContainer<Component_T>::insert(UComponent_T&& component)
    -> RecordIndex
{
    if (!m_optional.has_value()) {
        m_optional.emplace(std::forward<UComponent_T>(component));
    }
    return RecordIndex{ static_cast<RecordIndex::Underlying>(m_size++) };
}

template <modules::ecs::component_c Component_T>
    requires(std::is_empty_v<Component_T>)
constexpr auto ComponentContainer<Component_T>::remove(const RecordIndex record_index)
    -> Component_T
{
    PRECOND(record_index.underlying() < m_size);

    --m_size;

    return *m_optional;
}

template <modules::ecs::component_c Component_T>
    requires(std::is_empty_v<Component_T>)
constexpr auto ComponentContainer<Component_T>::erase(const RecordIndex record_index)
    -> std::optional<Component_T>
{
    if (record_index.underlying() >= m_size) {
        return std::nullopt;
    }

    --m_size;

    return *m_optional;
}

template <modules::ecs::component_c Component_T>
    requires(std::is_empty_v<Component_T>)
constexpr auto ComponentContainer<Component_T>::get(const RecordIndex record_index)
    -> Component_T&
{
    PRECOND(record_index.underlying() < m_size);

    return *m_optional;
}

template <modules::ecs::component_c Component_T>
    requires(std::is_empty_v<Component_T>)
constexpr auto ComponentContainer<Component_T>::get(const RecordIndex record_index) const
    -> const Component_T&
{
    return const_cast<ComponentContainer&>(*this).get(record_index);
}

template <modules::ecs::component_c Component_T>
    requires(std::is_empty_v<Component_T>)
constexpr auto ComponentContainer<Component_T>::empty() const noexcept -> bool
{
    return m_size == 0;
}

template <modules::ecs::component_c Component_T>
    requires(std::is_empty_v<Component_T>)
constexpr auto ComponentContainer<Component_T>::size() const noexcept -> std::size_t
{
    return m_size;
}

template <modules::ecs::component_c Component_T>
    requires(std::is_empty_v<Component_T>)
constexpr auto ComponentContainer<Component_T>::begin() -> Iterator<false>
{
    return Iterator<false>{ *this, 0 };
}

template <modules::ecs::component_c Component_T>
    requires(std::is_empty_v<Component_T>)
constexpr auto ComponentContainer<Component_T>::begin() const -> Iterator<true>
{
    return Iterator<true>{ *this, 0 };
}

template <modules::ecs::component_c Component_T>
    requires(std::is_empty_v<Component_T>)
constexpr auto ComponentContainer<Component_T>::end() -> Iterator<false>
{
    return Iterator<false>{ *this, m_size };
}

template <modules::ecs::component_c Component_T>
    requires(std::is_empty_v<Component_T>)
constexpr auto ComponentContainer<Component_T>::end() const -> Iterator<true>
{
    return Iterator<true>{ *this, m_size };
}
