module;

#include <memory>
#include <optional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module utility.containers.OptionalRef;

import utility.contracts;

namespace util {

export template <typename T>
    requires(!std::is_reference_v<T>)
class OptionalRef;

}   // namespace util

template <typename F, typename T>
concept and_then_func_c =
    !std::is_reference_v<T>
    && std::constructible_from<std::invoke_result_t<F, T&>, std::nullopt_t>;

template <typename F, typename T>
    requires(std::is_reference_v<T>)
using transform_result_t = std::conditional_t<
    std::is_reference_v<std::invoke_result_t<F, T>>,
    util::OptionalRef<std::remove_reference_t<std::invoke_result_t<F, T>>>,
    std::optional<std::invoke_result_t<F, T>>>;

template <typename F, typename T>
concept transform_func_c = std::invocable<F, T&>;

template <typename F, typename T>
concept or_else_func_c = std::constructible_from<std::invoke_result_t<F>, T&>;

namespace util {

template <typename T>
    requires(!std::is_reference_v<T>)
class OptionalRef {
public:
    OptionalRef() = default;
    constexpr explicit(false) OptionalRef(std::nullopt_t) noexcept;
    constexpr explicit(false
    ) OptionalRef(OptionalRef<std::remove_const_t<T>> other) noexcept
        requires(std::is_const_v<T>);
    constexpr explicit(false) OptionalRef(T& ref) noexcept;
    constexpr explicit(false) OptionalRef(
        const std::optional<std::reference_wrapper<T>>& optional_ref_wrapper
    ) noexcept;

    constexpr explicit(false) operator std::optional<std::remove_const_t<T>>(
    ) noexcept(noexcept(T{ std::declval<T&>() }));

    [[nodiscard]]
    constexpr auto operator->() const -> T*;
    [[nodiscard]]
    constexpr auto operator*() const -> T&;

    [[nodiscard]]
    constexpr auto has_value() const noexcept -> bool;

    [[nodiscard]]
    constexpr auto value_or(T& other) const noexcept -> T&;

    template <and_then_func_c<T> F>
    constexpr auto and_then(F&& func) const -> std::invoke_result_t<F, T&>;
    template <transform_func_c<T> F>
    constexpr auto transform(F&& func) const -> transform_result_t<F, T&>;
    template <or_else_func_c<T> F>
    constexpr auto or_else(F&& func) const -> std::invoke_result_t<F>;

private:
    T* m_handle{};
};

}   // namespace util

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr util::OptionalRef<T>::OptionalRef(std::nullopt_t) noexcept : OptionalRef{}
{}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr util::OptionalRef<T>::OptionalRef(
    OptionalRef<std::remove_const_t<T>> other
) noexcept
    requires(std::is_const_v<T>)
    : m_handle{ other.transform([](T& value) { return std::addressof(value); }
      ).value_or(nullptr) }
{}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr util::OptionalRef<T>::OptionalRef(T& ref) noexcept
    : m_handle{ std::addressof(ref) }
{}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr util::OptionalRef<T>::OptionalRef(
    const std::optional<std::reference_wrapper<T>>& optional_ref_wrapper
) noexcept
    : m_handle{ optional_ref_wrapper.transform([](T& ref) { return std::addressof(ref); }
      ).value_or(nullptr) }
{}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr util::OptionalRef<T>::operator std::optional<std::remove_const_t<T>>(
) noexcept(noexcept(T{ std::declval<T&>() }))
{
    if (has_value()) {
        return std::optional<T>{ *m_handle };
    }
    return std::optional<T>{};
}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr auto util::OptionalRef<T>::operator->() const -> T*
{
    PRECOND(m_handle != nullptr);
    return m_handle;
}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr auto util::OptionalRef<T>::operator*() const -> T&
{
    PRECOND(m_handle != nullptr);
    return *m_handle;
}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr auto util::OptionalRef<T>::has_value() const noexcept -> bool
{
    return m_handle != nullptr;
}

template <typename T>
    requires(!std::is_reference_v<T>)
constexpr auto util::OptionalRef<T>::value_or(T& other) const noexcept -> T&
{
    if (has_value()) {
        return *m_handle;
    }
    return other;
}

template <typename T>
    requires(!std::is_reference_v<T>)
template <and_then_func_c<T> F>
constexpr auto util::OptionalRef<T>::and_then(F&& func) const
    -> std::invoke_result_t<F, T&>
{
    if (has_value()) {
        return std::invoke(std::forward<F>(func), static_cast<T&>(*m_handle));
    }
    return std::nullopt;
}

template <typename T>
    requires(!std::is_reference_v<T>)
template <transform_func_c<T> F>
constexpr auto util::OptionalRef<T>::transform(F&& func) const
    -> transform_result_t<F, T&>
{
    if (has_value()) {
        return transform_result_t<F, T&>{
            std::invoke(std::forward<F>(func), static_cast<T&>(*m_handle))
        };
    }
    return std::nullopt;
}

template <typename T>
    requires(!std::is_reference_v<T>)
template <or_else_func_c<T> F>
constexpr auto util::OptionalRef<T>::or_else(F&& func) const -> std::invoke_result_t<F>
{
    if (!has_value()) {
        return std::invoke(std::forward<F>(func));
    }
    return { **this };
}
