#pragma once

#include <unordered_map>
#include <vector>

#include <entt/core/any.hpp>
#include <entt/core/type_info.hpp>

#include <optional_view.hpp>

namespace engine::utils {

class Context {
public:
    using Any       = entt::basic_any<0>;
    using TypeIndex = entt::id_type;

    Context()                   = default;
    Context(Context&&) noexcept = default;
    inline ~Context() noexcept;

    template <typename T>
    auto emplace(auto&&... t_args) noexcept
        -> std::add_lvalue_reference_t<std::remove_cv_t<T>>;

    template <typename T>
    [[nodiscard]] auto find() noexcept
        -> opview::optional_view<std::remove_cv_t<T>>;

    template <typename T>
    [[nodiscard]] auto contains() const noexcept -> bool;

private:
    std::unordered_map<TypeIndex, TypeIndex> m_index_map;
    std::vector<Any>                         m_elements;
};

}   // namespace engine::utils

#include "Context.inl"
