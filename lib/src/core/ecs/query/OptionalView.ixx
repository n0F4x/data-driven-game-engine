module;

#include <optional>
#include <ranges>
#include <span>

export module core.ecs:query.OptionalView;

import utility.Overloaded;

import :query.query_parameter_tags;

template <typename T>
class OptionalView : std::ranges::view_interface<OptionalView<T>> {
public:
    class Iterator;

    OptionalView() = default;

    constexpr explicit OptionalView(const std::span<T> view) : m_optional_view{ view } {}

    [[nodiscard]]
    auto begin() const -> Iterator
    {
        if (m_optional_view.empty()) {
            return Iterator{};
        }
        return Iterator{ std::ranges::begin(m_optional_view) };
    }

    [[nodiscard]]
    auto end() const -> Iterator
    {
        if (m_optional_view.empty()) {
            return Iterator{};
        }
        return Iterator{ std::ranges::end(m_optional_view) };
    }

private:
    std::span<T> m_optional_view;
};

template <typename T>
class OptionalView<T>::Iterator {
public:
    using iterator_concept = std::random_access_iterator_tag;
    using value_type       = core::ecs::Optional<T>;
    using difference_type  = typename std::span<T>::difference_type;

    Iterator() = default;

    [[nodiscard]]
    constexpr auto operator*() const -> core::ecs::Optional<T>
    {
        return m_optional_iterator
            .transform([](const typename std::span<T>::iterator iterator) {
                return core::ecs::Optional{ *iterator };
            })
            .value_or(std::nullopt);
    }

    [[nodiscard]]
    constexpr auto operator[](const difference_type difference) const
        -> core::ecs::Optional<T>
    {
        return m_optional_iterator
            .transform([difference](const typename std::span<T>::iterator iterator) {
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

    constexpr auto operator+=(const typename std::span<T>::difference_type difference)
        -> Iterator&
    {
        if (m_optional_iterator.has_value()) {
            (*m_optional_iterator) += difference;
        }
        return *this;
    }

    constexpr auto operator-=(const typename std::span<T>::difference_type difference)
        -> Iterator&
    {
        if (m_optional_iterator.has_value()) {
            (*m_optional_iterator) -= difference;
        }
        return *this;
    }

    [[nodiscard]]
    constexpr auto operator+(const typename std::span<T>::difference_type difference)
        -> Iterator
    {
        return auto{ *this } += difference;
    }

    [[nodiscard]]
    constexpr auto operator-(const typename std::span<T>::difference_type difference)
        -> Iterator
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

    constexpr explicit Iterator(const typename std::span<T>::iterator iterator)
        : m_optional_iterator{ iterator }
    {}

    std::optional<typename std::span<T>::iterator> m_optional_iterator;
};

static_assert(std::ranges::input_range<OptionalView<int>>);
