#pragma once

#include "core/graphics/model/Model.hpp"
#include "core/renderer/base/allocator/Allocator.hpp"
#include "core/renderer/base/descriptor_pool/DescriptorPool.hpp"
#include "core/renderer/material_system/Effect.hpp"

namespace core::renderer {

class RenderModel2 {
public:
    class Requirements;

    struct PipelineCreateInfo {
        Effect             effect;
        vk::PipelineLayout layout;
        vk::RenderPass     render_pass;
    };

    [[nodiscard]]
    static auto descriptor_pool_sizes() -> std::vector<vk::DescriptorPoolSize>;

    [[nodiscard]]
    static auto load(
        vk::Device                     t_device,
        const Allocator&               t_allocator,
        vk::DescriptorSetLayout        t_descriptor_set_layout,
        const PipelineCreateInfo&      t_pipeline_create_info,
        vk::DescriptorPool             t_descriptor_pool,
        vk::CommandBuffer              t_transfer_command_buffer,
        cache::Handle<graphics::Model> t_model
    ) -> RenderModel2;

    [[nodiscard]]
    static auto create_descriptor_set_layout(vk::Device t_device
    ) noexcept -> vk::UniqueDescriptorSetLayout;
    [[nodiscard]]
    static auto push_constant_range() noexcept -> vk::PushConstantRange;

    auto draw(
        vk::CommandBuffer  t_graphics_command_buffer,
        vk::PipelineLayout t_pipeline_layout
    ) const noexcept -> void;

private:
    MappedBuffer            m_uniform_buffer;
    vk::UniqueDescriptorSet m_descriptor_set;
    vk::UniquePipeline      m_pipeline;

    Buffer            m_vertex_buffer;
    vk::DeviceAddress m_vertex_buffer_address;
    Buffer            m_index_buffer;
    Buffer            m_transform_buffer;
    vk::DeviceAddress m_transform_buffer_address{};

    cache::Handle<graphics::Model> m_model;

    explicit RenderModel2(
        vk::Device                       t_device,
        MappedBuffer&&                   t_uniform_buffer,
        vk::UniqueDescriptorSet&&        t_descriptor_set,
        vk::UniquePipeline&&             t_pipeline,
        Buffer&&                         t_vertex_buffer,
        Buffer&&                         t_index_buffer,
        Buffer&&                         t_transform_buffer,
        cache::Handle<graphics::Model>&& t_model
    );
};

}   // namespace core::renderer
