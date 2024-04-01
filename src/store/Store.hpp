#pragma once

#include <typeindex>
#include <unordered_map>
#include <vector>

#include <tl/optional.hpp>

#include <entt/core/any.hpp>

class Store {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    Store()                 = default;
    Store(Store&&) noexcept = default;
    inline ~Store() noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    template <typename T>
    auto emplace(auto&&... t_args) -> T&;
    template <typename T>
    auto emplace_or_replace(auto&&... t_args) -> T&;

    template <typename T>
    [[nodiscard]] auto find() noexcept -> tl::optional<T&>;
    template <typename T>
    [[nodiscard]] auto find() const noexcept -> tl::optional<const T&>;

    template <typename T>
    [[nodiscard]] auto at() -> T&;
    template <typename T>
    [[nodiscard]] auto at() const -> const T&;

    template <typename T>
    [[nodiscard]] auto contains() const noexcept -> bool;

private:
    ///*************///
    ///  Variables  ///
    ///*************///
    std::unordered_map<std::type_index, size_t> m_index_map;
    std::vector<entt::any>                      m_elements;
};

#include "Store.inl"
