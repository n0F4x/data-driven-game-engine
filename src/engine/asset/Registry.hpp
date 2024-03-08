#pragma once

#include <unordered_map>

#include <entt/core/fwd.hpp>

#include "engine/common/Store.hpp"

namespace engine::asset {

template <typename IdType, template <typename...> typename ContainerTemplate>
class BasicRegistry {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    template <typename Resource>
    auto emplace(entt::id_type t_id, Resource&& t_resource) noexcept -> Resource&;

    template <typename Resource>
    [[nodiscard]] auto find(entt::id_type t_id) noexcept -> tl::optional<Resource&>;
    template <typename Resource>
    [[nodiscard]] auto find(entt::id_type t_id) const noexcept
        -> tl::optional<const Resource&>;

    template <typename Resource>
    [[nodiscard]] auto at(entt::id_type t_id) -> Resource&;
    template <typename Resource>
    [[nodiscard]] auto at(entt::id_type t_id) const -> const Resource&;

private:
    ///****************///
    ///  Type aliases  ///
    ///****************///
    template <typename Resource>
    using ContainerType = ContainerTemplate<IdType, Resource>;

    ///*************///
    ///  Variables  ///
    ///*************///
    Store m_store;
};

using Registry = BasicRegistry<entt::id_type, std::unordered_map>;

}   // namespace engine::asset

#include "Registry.inl"
