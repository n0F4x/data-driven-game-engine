#pragma once

#include <vulkan/vulkan.hpp>

#include "core/gltf/RenderModel.hpp"
#include "core/graphics/Camera.hpp"
#include "core/renderer/base/descriptor_pool/DescriptorPool.hpp"

namespace core::renderer {

class Scene {
public:
    class Builder;

    [[nodiscard]]
    static auto create() noexcept -> Builder;

    auto draw(vk::CommandBuffer graphics_command_buffer, const graphics::Camera& camera)
        const -> void;

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
    DescriptorPool                               m_descriptor_pool;

    RandomAccessBuffer<ShaderScene> m_global_buffer;
    vk::UniqueDescriptorSet         m_global_descriptor_set;

    std::vector<gltf::RenderModel> m_models;

    explicit Scene(
        vk::UniqueDescriptorSetLayout&&                global_descriptor_set_layout,
        std::array<vk::UniqueDescriptorSetLayout, 3>&& model_descriptor_set_layouts,
        vk::UniquePipelineLayout&&                     pipeline_layout,
        DescriptorPool&&                               descriptor_pool,
        RandomAccessBuffer<ShaderScene>&&              global_buffer,
        vk::UniqueDescriptorSet&&                      global_descriptor_set,
        std::vector<gltf::RenderModel>&&               models
    ) noexcept;
};

}   // namespace core::renderer
