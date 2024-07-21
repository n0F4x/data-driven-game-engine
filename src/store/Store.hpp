#pragma once

#include <functional>
#include <optional>
#include <typeindex>

#include <tsl/ordered_map.h>

#include <entt/core/any.hpp>

class Store {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    Store()                 = default;
    Store(const Store&)     = delete;
    Store(Store&&) noexcept = default;
    ~Store() noexcept;

    auto operator=(const Store&) -> Store&     = delete;
    auto operator=(Store&&) noexcept -> Store& = default;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    template <typename T>
    auto emplace(auto&&... t_args) -> T&;

    template <typename T>
    [[nodiscard]]
    auto find() noexcept -> std::optional<std::reference_wrapper<T>>;
    template <typename T>
    [[nodiscard]]
    auto find() const noexcept -> std::optional<std::reference_wrapper<const T>>;

    template <typename T>
    [[nodiscard]]
    auto at() -> T&;
    template <typename T>
    [[nodiscard]]
    auto at() const -> const T&;

    template <typename T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    tsl::ordered_map<std::type_index, entt::any> m_map;
};

#include "Store.inl"
