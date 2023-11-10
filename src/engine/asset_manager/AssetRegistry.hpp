#pragma once

#include <unordered_map>

#include "engine/common/Store.hpp"

namespace engine::asset_manager {

class AssetRegistry {
public:
    ///-----------///
    ///  Methods  ///
    ///-----------///
    template <typename Asset>
    auto emplace(Asset&& t_asset, entt::id_type t_id) noexcept -> Asset&;

    template <typename Asset>
    [[nodiscard]] auto find(entt::id_type t_id) noexcept
        -> tl::optional<Asset&>;

private:
    ///****************///
    ///  Type aliases  ///
    ///****************///
    template <typename Asset>
    using ContainerType = std::unordered_map<entt::id_type, Asset>;

    ///*************///
    ///  Variables  ///
    ///*************///
    Store m_store;
};

}   // namespace engine::asset_manager

#include "AssetRegistry.inl"
