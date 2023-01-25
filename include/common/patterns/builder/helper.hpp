#pragma once

#include <type_traits>
#include <utility>

#define Self std::remove_reference_t<decltype(*this)>

template <class Product>
class BuilderBase {
    friend Product;

protected:
    [[nodiscard]] constexpr BuilderBase() noexcept = default;
    [[nodiscard]] constexpr BuilderBase(const BuilderBase&) = delete;
    [[nodiscard]] constexpr BuilderBase(BuilderBase&&) noexcept = default;

    template <typename... Args>
    [[nodiscard]] constexpr explicit BuilderBase(Args&&... t_args) noexcept
        : m_draft{ std::forward<Args>(t_args)... } {}

    [[nodiscard]] constexpr auto draft() -> Product& {
        return m_draft;
    }

public:
    [[nodiscard]] constexpr explicit(false) operator Product() noexcept {
        return build();
    }

    [[nodiscard]] constexpr auto build() noexcept {
        return std::move(m_draft);
    }

private:
    Product m_draft;
};
