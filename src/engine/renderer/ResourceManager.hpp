#pragma once

#include <functional>
#include <unordered_map>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include <entt/core/fwd.hpp>

#include "engine/renderer/base/Allocator.hpp"
#include "engine/renderer/material_system/Texture.hpp"
#include "engine/renderer/model/RenderModel.hpp"

namespace engine::renderer {

class ResourceManager {
public:
    explicit ResourceManager(const Allocator& t_allocator) noexcept;

    auto add_model(entt::id_type t_id, RenderModel&& t_model_info) -> RenderModel&;
    [[nodiscard]] auto find_model(entt::id_type t_id) const noexcept
        -> tl::optional<RenderModel&>;
    auto remove_model(entt::id_type) -> RenderModel;

    auto add_image(entt::id_type t_id, vk::UniqueImage&& t_image) noexcept
        -> vk::UniqueImage&;
    [[nodiscard]] auto find_image(entt::id_type t_id) const noexcept
        -> tl::optional<vk::UniqueImage&>;
    auto remove_image(entt::id_type) -> vk::UniqueImage;

private:
    std::reference_wrapper<const Allocator>  m_allocator;
    std::unordered_map<entt::id_type, RenderModel> m_models;
};

}   // namespace engine::renderer
