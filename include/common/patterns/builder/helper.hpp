#pragma once

#include <type_traits>
#include <utility>

template <class Product>
class BuilderBase {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Product = Product;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] constexpr BuilderBase() = default;
    [[nodiscard]] constexpr BuilderBase(const BuilderBase&) noexcept = default;
    [[nodiscard]] constexpr BuilderBase(BuilderBase&&) noexcept = default;
    BuilderBase& operator=(const BuilderBase&) noexcept = default;
    BuilderBase& operator=(BuilderBase&&) noexcept = default;

    template <typename... Args>
        requires std::constructible_from<Product, Args...>
    [[nodiscard]] constexpr explicit BuilderBase(std::in_place_t,
                                                 Args&&... t_args)
        : m_draft{ std::forward<Args>(t_args)... } {}

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] constexpr auto draft() -> Product& {
        return m_draft;
    }

    [[nodiscard]] constexpr explicit(false) operator Product() noexcept {
        return build();
    }

    [[nodiscard]] constexpr auto build() noexcept {
        return std::move(m_draft);
    }

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    Product m_draft;
};
