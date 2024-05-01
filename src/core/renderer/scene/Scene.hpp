#pragma once

#include <vulkan/vulkan.hpp>

#include "core/cache/Handle.hpp"
#include "core/graphics/camera/Camera.hpp"
#include "core/graphics/model/Model.hpp"
#include "core/renderer/base/descriptor_pool/DescriptorPool.hpp"
#include "core/renderer/model/RenderModel2.hpp"

namespace core::renderer {

class Scene {
public:
    class Builder;

    [[nodiscard]]
    static auto create() noexcept -> Builder;

    auto draw(
        vk::CommandBuffer       t_graphics_command_buffer,
        const graphics::Camera& t_camera
    ) const -> void;

private:
    struct GlobalUniformBlock {
        graphics::Camera camera;
    };

    friend Builder;

    vk::UniqueDescriptorSetLayout m_global_descriptor_set_layout;
    vk::UniqueDescriptorSetLayout m_model_descriptor_set_layout;
    vk::UniquePipelineLayout      m_pipeline_layout;
    DescriptorPool                m_descriptor_pool;

    MappedBuffer            m_global_buffer;
    vk::UniqueDescriptorSet m_global_descriptor_set;

    std::vector<RenderModel2> m_models;

    explicit Scene(
        vk::UniqueDescriptorSetLayout&& t_global_descriptor_set_layout,
        vk::UniqueDescriptorSetLayout&& t_model_descriptor_set_layout,
        vk::UniquePipelineLayout&&      t_pipeline_layout,
        DescriptorPool&&                t_descriptor_pool,
        MappedBuffer&&                  t_global_buffer,
        vk::UniqueDescriptorSet&&       t_global_descriptor_set,
        std::vector<RenderModel2>&&     t_models
    ) noexcept;
};

}   // namespace core::renderer
