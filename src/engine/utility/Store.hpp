#pragma once

#include <unordered_map>
#include <vector>

#include <tl/optional.hpp>

#include <entt/core/any.hpp>
#include <entt/core/type_info.hpp>

namespace engine {

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
    auto emplace(auto&&... t_args) noexcept -> T&;

    template <typename T>
    [[nodiscard]] auto find() noexcept -> tl::optional<T&>;

    template <typename T>
    [[nodiscard]] auto contains() const noexcept -> bool;

private:
    ///****************///
    ///  Type aliases  ///
    ///****************///
    using Any       = entt::basic_any<0>;
    using TypeIndex = entt::id_type;

    ///*************///
    ///  Variables  ///
    ///*************///
    std::unordered_map<TypeIndex, TypeIndex> m_index_map;
    std::vector<Any>                         m_elements;
};

}   // namespace engine

#include "Store.inl"
