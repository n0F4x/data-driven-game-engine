#pragma once

#include <functional>
#include <unordered_map>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include <entt/core/fwd.hpp>

#include "engine/renderer/base/Allocator.hpp"
#include "engine/renderer/material_system/Texture.hpp"
#include "engine/renderer/model/Model.hpp"

namespace engine::renderer {

class ResourceManager {
public:
    explicit ResourceManager(const Allocator& t_allocator) noexcept;

    auto add_model(entt::id_type t_id, Model&& t_model_info) -> Model&;
    [[nodiscard]] auto find_model(entt::id_type t_id) const noexcept
        -> tl::optional<Model&>;
    auto remove_model(entt::id_type) -> Model;

    auto add_image(entt::id_type t_id, vk::UniqueImage&& t_image) noexcept
        -> vk::UniqueImage&;
    [[nodiscard]] auto find_image(entt::id_type t_id) const noexcept
        -> tl::optional<vk::UniqueImage&>;
    auto remove_image(entt::id_type) -> vk::UniqueImage;

private:
    std::reference_wrapper<const Allocator>  m_allocator;
    std::unordered_map<entt::id_type, Model> m_models;
};

}   // namespace engine::renderer
