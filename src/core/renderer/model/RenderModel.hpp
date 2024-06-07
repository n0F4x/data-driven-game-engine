#pragma once

#include <future>

#include "core/gltf/Model.hpp"
#include "core/renderer/base/allocator/Allocator.hpp"
#include "core/renderer/base/descriptor_pool/DescriptorPool.hpp"
#include "core/renderer/material_system/Effect.hpp"

namespace core::renderer {

class RenderModel {
public:
    class Requirements;

    struct DescriptorSetLayoutCreateInfo {
        uint32_t max_image_count;
        uint32_t max_sampler_count;
    };

    struct PipelineCreateInfo {
        Effect             effect;
        vk::PipelineLayout layout;
        vk::RenderPass     render_pass;
    };

    [[nodiscard]]
    static auto descriptor_set_count() noexcept -> uint32_t;
    [[nodiscard]]
    static auto descriptor_pool_sizes(const DescriptorSetLayoutCreateInfo& info
    ) -> std::vector<vk::DescriptorPoolSize>;

    [[nodiscard]]
    static auto create_loader(
        vk::Device                                  device,
        const Allocator&                            allocator,
        std::span<const vk::DescriptorSetLayout, 3> descriptor_set_layouts,
        const PipelineCreateInfo&                   pipeline_create_info,
        vk::DescriptorPool                          descriptor_pool,
        cache::Handle<gltf::Model>                  model,
        cache::Cache&                               cache
    ) -> std::packaged_task<RenderModel(vk::CommandBuffer)>;

    [[nodiscard]]
    static auto create_descriptor_set_layouts(
        vk::Device                           t_device,
        const DescriptorSetLayoutCreateInfo& info
    ) -> std::array<vk::UniqueDescriptorSetLayout, 3>;
    [[nodiscard]]
    static auto push_constant_range() noexcept -> vk::PushConstantRange;

    auto draw(
        vk::CommandBuffer  t_graphics_command_buffer,
        vk::PipelineLayout t_pipeline_layout
    ) const noexcept -> void;

private:
    struct Mesh {
        struct Primitive {
            cache::Handle<vk::UniquePipeline> pipeline;
            std::optional<uint32_t>           material_index;
            uint32_t                          first_index_index;
            uint32_t                          index_count;
            uint32_t                          vertex_count;
        };

        std::vector<Primitive> primitives;
    };

    Buffer m_index_buffer;

    // Base descriptor set
    Buffer            m_vertex_buffer;
    vk::DeviceAddress m_vertex_buffer_address;
    MappedBuffer      m_vertex_uniform;

    Buffer            m_transform_buffer;
    vk::DeviceAddress m_transform_buffer_address;
    MappedBuffer      m_transform_uniform;

    vk::UniqueSampler m_default_sampler;

    Buffer            m_texture_buffer;
    vk::DeviceAddress m_texture_buffer_address;
    MappedBuffer      m_texture_uniform;

    MappedBuffer m_default_material_uniform;

    Buffer            m_material_buffer;
    vk::DeviceAddress m_material_buffer_address;
    MappedBuffer      m_material_uniform;

    vk::UniqueDescriptorSet m_base_descriptor_set;

    // Image descriptor set
    std::vector<Image>               m_images;
    std::vector<vk::UniqueImageView> m_image_views;
    vk::UniqueDescriptorSet          m_image_descriptor_set;

    // Sampler descriptor set
    std::vector<vk::UniqueSampler> m_samplers;
    vk::UniqueDescriptorSet        m_sampler_descriptor_set;

    // Pipelines
    std::vector<Mesh> m_meshes;


    explicit RenderModel(
        vk::Device                         device,
        Buffer&&                           index_buffer,
        Buffer&&                           vertex_buffer,
        MappedBuffer&&                     vertex_uniform,
        Buffer&&                           transform_buffer,
        MappedBuffer&&                     transform_uniform,
        vk::UniqueSampler&&                default_sampler,
        Buffer&&                           texture_buffer,
        MappedBuffer&&                     texture_uniform,
        MappedBuffer&&                     default_material_uniform,
        Buffer&&                           material_buffer,
        MappedBuffer&&                     material_uniform,
        vk::UniqueDescriptorSet&&          base_descriptor_set,
        std::vector<Image>&&               images,
        std::vector<vk::UniqueImageView>&& image_views,
        vk::UniqueDescriptorSet&&          image_descriptor_set,
        std::vector<vk::UniqueSampler>&&   samplers,
        vk::UniqueDescriptorSet&&          sampler_descriptor_set,
        std::vector<Mesh>&&                meshes
    );
};

}   // namespace core::renderer
