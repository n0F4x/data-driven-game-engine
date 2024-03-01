#pragma once

#include <unordered_map>

#include <tl/optional.hpp>

#include <entt/core/fwd.hpp>

#include "engine/renderer/material_system/Texture.hpp"

namespace engine::renderer {

class ResourceManager {
public:
    auto add_image(entt::id_type t_id, vk::UniqueImage&& t_texture) -> vk::UniqueImage&;
    [[nodiscard]] auto find_image(entt::id_type t_id) const noexcept
        -> tl::optional<vk::UniqueImage&>;
    auto remove_image(entt::id_type) -> void;

private:
    std::unordered_map<entt::id_type, vk::UniqueImage> m_images;
};

}   // namespace engine::renderer
