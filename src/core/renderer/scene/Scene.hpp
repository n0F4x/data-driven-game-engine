#pragma once

#include <vulkan/vulkan.hpp>

#include <glm/ext/matrix_float4x4.hpp>

#include <core/renderer/resources/RandomAccessBuffer.hpp>

#include "core/renderer/base/descriptor_pool/DescriptorPool.hpp"

import core.gfx.Camera;
import core.gltf.RenderModel;

namespace core::renderer {

namespace base {

class Allocator;

}   // namespace base

class Scene {
public:
    class Builder;

    [[nodiscard]]
    static auto create() noexcept -> Builder;

    auto update(
        const gfx::Camera&               camera,
        const renderer::base::Allocator& allocator,
        vk::Queue                        sparse_queue,
        vk::CommandBuffer                transfer_command_buffer
    ) -> void;

    auto draw(vk::CommandBuffer graphics_command_buffer, const gfx::Camera& camera) const
        -> void;

private:
    struct ShaderScene {
        struct Camera {
            glm::vec4 position;
            glm::mat4 view;
            glm::mat4 projection;
        };

        Camera camera;
    };

    friend Builder;

    vk::UniqueDescriptorSetLayout                m_global_descriptor_set_layout;
    std::array<vk::UniqueDescriptorSetLayout, 3> m_model_descriptor_set_layouts;
    vk::UniquePipelineLayout                     m_pipeline_layout;
    base::DescriptorPool                         m_descriptor_pool;

    resources::RandomAccessBuffer<ShaderScene> m_global_buffer;
    vk::UniqueDescriptorSet                    m_global_descriptor_set;

    std::vector<gltf::RenderModel> m_models;

    explicit Scene(
        vk::UniqueDescriptorSetLayout&&                global_descriptor_set_layout,
        std::array<vk::UniqueDescriptorSetLayout, 3>&& model_descriptor_set_layouts,
        vk::UniquePipelineLayout&&                     pipeline_layout,
        base::DescriptorPool&&                         descriptor_pool,
        resources::RandomAccessBuffer<ShaderScene>&&   global_buffer,
        vk::UniqueDescriptorSet&&                      global_descriptor_set,
        std::vector<gltf::RenderModel>&&               models
    ) noexcept;
};

}   // namespace core::renderer
