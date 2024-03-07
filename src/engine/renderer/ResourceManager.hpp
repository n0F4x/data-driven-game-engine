#pragma once

#include <functional>
#include <unordered_map>

#include <tl/optional.hpp>

#include <vulkan/vulkan.hpp>

#include <entt/core/fwd.hpp>

#include "engine/renderer/base/Allocator.hpp"
#include "engine/renderer/material_system/Texture.hpp"
#include "engine/renderer/model/ImageInfo.hpp"
#include "engine/renderer/model/Model.hpp"
#include "engine/renderer/model/ModelInfo.hpp"

namespace engine::renderer {

class ResourceManager {
public:
    explicit ResourceManager(const Allocator& t_allocator) noexcept;

    auto load_model(
        const ModelInfo& t_model_info,
        vk::Device       t_device,
        vk::CommandPool  t_transfer_pool
    ) -> tl::optional<Model&>;
    [[nodiscard]] auto find_model(entt::id_type t_id) const noexcept
        -> tl::optional<Model&>;
    auto remove_model(entt::id_type) -> void;

    auto load_image(const ImageInfo& t_image_info) -> tl::optional<vk::UniqueImage&>;
    [[nodiscard]] auto find_image(entt::id_type t_id) const noexcept
        -> tl::optional<vk::UniqueImage&>;
    auto remove_image(entt::id_type) -> void;

private:
    std::reference_wrapper<const Allocator>  m_allocator;
    std::unordered_map<entt::id_type, Model> m_models;
};

}   // namespace engine::renderer
