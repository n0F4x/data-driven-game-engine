#pragma once

#include <utility>
#include <type_traits>
#include <concepts>


template<class Product>
class BuilderBase {
    friend Product;

protected:
    [[nodiscard]] constexpr BuilderBase() noexcept = default;
    [[nodiscard]] constexpr BuilderBase(const BuilderBase&) = delete;
    [[nodiscard]] constexpr BuilderBase(BuilderBase&&) noexcept = default;

    template<typename... Args>
    [[nodiscard]] constexpr explicit BuilderBase(Args&&... args) noexcept
        : product{ std::forward<Args>(args)... } {}

    [[nodiscard]] constexpr auto draft() -> Product& {
        return product;
    }

public:
    [[nodiscard]] constexpr explicit(false) operator Product() noexcept {
        return build();
    }
    [[nodiscard]] constexpr auto build() noexcept {
        return std::move(product);
    }

private:
    Product product;
};
