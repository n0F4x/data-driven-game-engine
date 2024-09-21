#pragma once

#include <future>

#include "core/cache/Cache.hpp"
#include "core/renderer/base/allocator/Allocator.hpp"
#include "core/renderer/memory/Image.hpp"
#include "core/renderer/memory/MappedBuffer.hpp"
#include "core/renderer/model/Drawable.hpp"

#include "Model.hpp"

struct ShaderMaterial;

namespace core::gltf {

class RenderModel : public renderer::Drawable {
public:
    struct PipelineCreateInfo {
        vk::PipelineLayout layout;
        vk::RenderPass     render_pass;
    };

    [[nodiscard]]
    static auto create_loader(
        vk::Device                                  device,
        const renderer::Allocator&                  allocator,
        std::span<const vk::DescriptorSetLayout, 3> descriptor_set_layouts,
        const PipelineCreateInfo&                   pipeline_create_info,
        vk::DescriptorPool                          descriptor_pool,
        const Model&                                model,
        cache::Cache&                               cache
    ) -> std::packaged_task<RenderModel(vk::CommandBuffer)>;

    auto draw(
        vk::CommandBuffer  graphics_command_buffer,
        vk::PipelineLayout pipeline_layout
    ) const noexcept -> void final;

private:
    struct Mesh {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init, hicpp-member-init)
        struct Primitive {
            cache::Handle<vk::UniquePipeline> pipeline;
            std::optional<uint32_t>           material_index;
            uint32_t                          first_index_index;
            uint32_t                          index_count;
        };

        std::vector<Primitive> primitives{};
    };

    renderer::Buffer m_index_buffer;

    // Base descriptor set
    renderer::Buffer                                m_vertex_buffer;
    vk::DeviceAddress                               m_vertex_buffer_address;
    renderer::RandomAccessBuffer<vk::DeviceAddress> m_vertex_uniform;

    renderer::Buffer                                m_transform_buffer;
    vk::DeviceAddress                               m_transform_buffer_address;
    renderer::RandomAccessBuffer<vk::DeviceAddress> m_transform_uniform;

    vk::UniqueSampler m_default_sampler;

    renderer::Buffer                                m_texture_buffer;
    vk::DeviceAddress                               m_texture_buffer_address;
    renderer::RandomAccessBuffer<vk::DeviceAddress> m_texture_uniform;

    renderer::RandomAccessBuffer<ShaderMaterial> m_default_material_uniform;

    renderer::Buffer                                m_material_buffer;
    vk::DeviceAddress                               m_material_buffer_address;
    renderer::RandomAccessBuffer<vk::DeviceAddress> m_material_uniform;

    vk::UniqueDescriptorSet m_base_descriptor_set;

    // Image descriptor set
    std::vector<renderer::Image>     m_images;
    std::vector<vk::UniqueImageView> m_image_views;
    vk::UniqueDescriptorSet          m_image_descriptor_set;

    // Sampler descriptor set
    std::vector<vk::UniqueSampler> m_samplers;
    vk::UniqueDescriptorSet        m_sampler_descriptor_set;

    // Pipelines
    std::vector<Mesh> m_meshes;


    explicit RenderModel(
        vk::Device                                        device,
        renderer::Buffer&&                                index_buffer,
        renderer::Buffer&&                                vertex_buffer,
        renderer::RandomAccessBuffer<vk::DeviceAddress>&& vertex_uniform,
        renderer::Buffer&&                                transform_buffer,
        renderer::RandomAccessBuffer<vk::DeviceAddress>&& transform_uniform,
        vk::UniqueSampler&&                               default_sampler,
        renderer::Buffer&&                                texture_buffer,
        renderer::RandomAccessBuffer<vk::DeviceAddress>&& texture_uniform,
        renderer::RandomAccessBuffer<ShaderMaterial>&&    default_material_uniform,
        renderer::Buffer&&                                material_buffer,
        renderer::RandomAccessBuffer<vk::DeviceAddress>&& material_uniform,
        vk::UniqueDescriptorSet&&                         base_descriptor_set,
        std::vector<renderer::Image>&&                    images,
        std::vector<vk::UniqueImageView>&&                image_views,
        vk::UniqueDescriptorSet&&                         image_descriptor_set,
        std::vector<vk::UniqueSampler>&&                  samplers,
        vk::UniqueDescriptorSet&&                         sampler_descriptor_set,
        std::vector<Mesh>&&                               meshes
    );
};

}   // namespace core::gltf
