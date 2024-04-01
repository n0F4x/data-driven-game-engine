#pragma once

#include <typeindex>
#include <unordered_map>
#include <vector>

#include <tl/optional.hpp>

#include <tsl/ordered_map.h>

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
    using Key       = std::type_index;
    using Value     = entt::any;
    using Allocator = std::allocator<std::pair<Key, Value>>;

    ///*************///
    ///  Variables  ///
    ///*************///
    tsl::ordered_map<
        Key,
        Value,
        std::hash<Key>,
        std::equal_to<>,
        Allocator,
        std::vector<std::pair<Key, Value>, Allocator>>
        m_map;
};

#include "Store.inl"
