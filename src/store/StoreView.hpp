#pragma once

#include "Store.hpp"

class StoreView {
public:
    explicit StoreView(Store& store) noexcept;

    template <typename T>
    [[nodiscard]]
    auto find() const noexcept -> std::optional<std::reference_wrapper<T>>;

    template <typename T>
    [[nodiscard]]
    auto at() const -> T&;

    template <typename T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

private:
    std::reference_wrapper<Store> m_store;
};

#include "StoreView.inl"
