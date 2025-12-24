module;

#include <concepts>
#include <optional>
#include <type_traits>

export module ddge.utility.Bool;

import ddge.utility.containers.OptionalRef;
import ddge.utility.meta.concepts.specialization_of;

namespace ddge::util {

template <typename F>
concept and_then_func_c =
    meta::specialization_of_c<std::invoke_result_t<F&&>, std::optional>
    || meta::specialization_of_c<std::invoke_result_t<F&&>, OptionalRef>;

template <typename F>
using transform_result_t = std::conditional_t<
    std::is_reference_v<std::invoke_result_t<F>>,
    OptionalRef<std::remove_reference_t<std::invoke_result_t<F>>>,
    std::optional<std::invoke_result_t<F>>>;

template <typename F>
concept transform_func_c = std::invocable<F&&>;

export class Bool {
public:
    Bool() = default;
    constexpr explicit(false) Bool(std::nullopt_t) noexcept;
    constexpr explicit Bool(bool value) noexcept;

    constexpr auto operator=(bool value) noexcept -> Bool&;

    constexpr explicit operator bool() const noexcept;

    template <and_then_func_c F>
    constexpr auto and_then(F&& func) const -> std::invoke_result_t<F&&>;
    template <transform_func_c F>
    constexpr auto transform(F&& func) const -> transform_result_t<F&&>;

private:
    bool m_value{};
};

}   // namespace ddge::util

namespace ddge::util {

constexpr Bool::Bool(std::nullopt_t) noexcept {}

constexpr Bool::Bool(const bool value) noexcept : m_value{ value } {}

constexpr auto Bool::operator=(const bool value) noexcept -> Bool&
{
    m_value = value;
    return *this;
}

constexpr Bool::operator bool() const noexcept
{
    return m_value;
}

template <and_then_func_c F>
constexpr auto Bool::and_then(F&& func) const -> std::invoke_result_t<F&&>
{
    if (*this) {
        return std::invoke(std::forward<F>(func));
    }
    return std::nullopt;
}

template <transform_func_c F>
constexpr auto Bool::transform(F&& func) const -> transform_result_t<F&&>
{
    if (*this) {
        return transform_result_t<F&&>{ std::invoke(std::forward<F>(func)) };
    }
    return std::nullopt;
}

}   // namespace ddge::util
