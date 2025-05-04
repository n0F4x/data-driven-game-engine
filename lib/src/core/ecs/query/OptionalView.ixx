module;

#include <optional>
#include <ranges>
#include <type_traits>

#include <utility/contracts.hpp>

export module core.ecs:query.OptionalView;

import utility.meta.type_traits.const_like;

import :component_c;
import :ComponentContainer;
import :query.query_parameter_tags;

template <typename Container_T>
[[nodiscard]]
constexpr auto empty_container_ref() -> Container_T&
{
    static Container_T empty_container;
    return empty_container;
}

template <typename Component_T>
    requires core::ecs::component_c<std::remove_const_t<Component_T>>
class OptionalView : std::ranges::view_interface<OptionalView<Component_T>> {
    using Container = util::meta::
        const_like_t<ComponentContainer<std::remove_const_t<Component_T>>, Component_T>;
    using Underlying = std::ranges::ref_view<Container>;

public:
    class Iterator;

    OptionalView() = default;

    constexpr explicit OptionalView(Container& container) : m_ref_view{ container }
    {
        PRECOND(!container.empty());
    }

    [[nodiscard]]
    auto begin() const -> Iterator
    {
        if (m_ref_view.empty()) {
            return Iterator{};
        }
        return Iterator{ std::ranges::begin(m_ref_view) };
    }

    [[nodiscard]]
    auto end() const -> Iterator
    {
        if (m_ref_view.empty()) {
            return Iterator{};
        }
        return Iterator{ std::ranges::end(m_ref_view) };
    }

private:
    Underlying m_ref_view{ empty_container_ref<Container>() };
};

template <typename Component_T>
    requires core::ecs::component_c<std::remove_const_t<Component_T>>
class OptionalView<Component_T>::Iterator {
    using UnderlyingIterator = std::ranges::iterator_t<Container>;

public:
    using iterator_concept = std::random_access_iterator_tag;
    using value_type       = core::ecs::Optional<Component_T>;
    using difference_type  = typename UnderlyingIterator::difference_type;

    Iterator() = default;

    [[nodiscard]]
    constexpr auto operator*() const -> core::ecs::Optional<Component_T>
    {
        return m_optional_iterator
            .transform([](const UnderlyingIterator iterator) {
                return core::ecs::Optional{ *iterator };
            })
            .value_or(std::nullopt);
    }

    [[nodiscard]]
    constexpr auto operator[](const difference_type difference) const
        -> core::ecs::Optional<Component_T>
    {
        return m_optional_iterator
            .transform([difference](const UnderlyingIterator iterator) {
                return core::ecs::Optional{ *iterator[difference] };
            })
            .value_or(std::nullopt);
    }

    constexpr auto operator++() -> Iterator&
    {
        if (m_optional_iterator.has_value()) {
            ++(*m_optional_iterator);
        }
        return *this;
    }

    constexpr auto operator++(int) -> Iterator
    {
        Iterator result{ *this };
        ++(*this);
        return result;
    }

    constexpr auto operator--() -> Iterator&
    {
        if (m_optional_iterator.has_value()) {
            --(*m_optional_iterator);
        }
        return *this;
    }

    constexpr auto operator--(int) -> Iterator
    {
        Iterator result{ *this };
        --(*this);
        return result;
    }

    constexpr auto operator+=(const difference_type difference) -> Iterator&
    {
        if (m_optional_iterator.has_value()) {
            (*m_optional_iterator) += difference;
        }
        return *this;
    }

    constexpr auto operator-=(const difference_type difference) -> Iterator&
    {
        if (m_optional_iterator.has_value()) {
            (*m_optional_iterator) -= difference;
        }
        return *this;
    }

    [[nodiscard]]
    constexpr auto operator+(const difference_type difference) -> Iterator
    {
        return auto{ *this } += difference;
    }

    [[nodiscard]]
    constexpr auto operator-(const difference_type difference) -> Iterator
    {
        return auto{ *this } -= difference;
    }

    [[nodiscard]]
    constexpr auto operator==(const Iterator& other) const -> bool
    {
        if (m_optional_iterator.has_value() && other.m_optional_iterator.has_value()) {
            return *m_optional_iterator == *other.m_optional_iterator;
        }
        return false;
    };

    [[nodiscard]]
    constexpr auto operator<=>(const Iterator&) const = default;

private:
    friend OptionalView;

    constexpr explicit Iterator(const UnderlyingIterator iterator)
        : m_optional_iterator{ iterator }
    {}

    std::optional<UnderlyingIterator> m_optional_iterator;
};

static_assert(std::ranges::input_range<OptionalView<int>>);
