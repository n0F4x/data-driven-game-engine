module;

#include <optional>
#include <type_traits>

export module ddge.modules.ecs:query.query_filter_tags;

import ddge.utility.containers.OptionalRef;

import :component_c;
import :query.queryable_component_c;
import :query.QueryFilterTagBase;

namespace ddge::ecs::inline query_filter_tags {

export template <component_c>
struct With : ::QueryFilterTagBase {};

export template <component_c>
struct Without : ::QueryFilterTagBase {};

export template <typename T>
    requires component_c<std::remove_const_t<T>>
class Optional : public ::QueryFilterTagBase, public util::OptionalRef<T> {
    using util::OptionalRef<T>::OptionalRef;
    using util::OptionalRef<T>::operator std::optional<std::remove_const_t<T>>;
};

// TODO: remove deduction guide with P2582
export template <typename T>
Optional(T&) -> Optional<T>;

export template <typename T>
    requires component_c<std::remove_const_t<T>>
          && (!queryable_component_c<std::remove_const_t<T>>)
class Optional<T> : public QueryFilterTagBase {
public:
    using ValueType = T;

    Optional() = default;

    constexpr explicit(false) Optional(std::nullopt_t) noexcept {}

    template <std::same_as<std::remove_const_t<T>> U>
        requires(std::is_const_v<T>)
    constexpr explicit(false) Optional(Optional<U> other) noexcept
        : m_has_value{ other.m_has_value }
    {}

    constexpr explicit(false) Optional(T&) noexcept : m_has_value{ true } {}

    constexpr explicit(false) Optional(
        const std::optional<std::reference_wrapper<T>>& optional_ref_wrapper
    ) noexcept
        : m_has_value{ optional_ref_wrapper.has_value() }
    {}

    [[nodiscard]]
    constexpr auto has_value() const noexcept -> bool
    {
        return m_has_value;
    }

private:
    bool m_has_value{};
};

}   // namespace ddge::ecs::inline query_filter_tags
