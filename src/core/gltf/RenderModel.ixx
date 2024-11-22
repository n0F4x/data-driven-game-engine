module;

#include <functional>
#include <future>
#include <variant>

#include "core/renderer/base/allocator/Allocator.hpp"
#include "core/renderer/model/Drawable.hpp"
#include "core/renderer/resources/Buffer.hpp"
#include "core/renderer/resources/RandomAccessBuffer.hpp"

export module core.gltf.RenderModel;

import core.cache.Cache;
import core.cache.Handle;
import core.gfx.Camera;
import core.gfx.resources.VirtualImage;
import core.gfx.resources.Image;

import core.gltf.Model;

struct ShaderMaterial;

namespace core::gltf {

export class RenderModel : public renderer::Drawable {
public:
    struct PipelineCreateInfo {
        vk::PipelineLayout layout;
        vk::RenderPass     render_pass;
    };

    [[nodiscard]]
    static auto create_loader(
        const renderer::base::Device&               device,
        const renderer::base::Allocator&            allocator,
        std::span<const vk::DescriptorSetLayout, 3> descriptor_set_layouts,
        const PipelineCreateInfo&                   pipeline_create_info,
        vk::DescriptorPool                          descriptor_pool,
        const cache::Handle<const Model>&           model,
        cache::Cache&                               cache,
        bool                                        use_virtual_images
    ) -> std::packaged_task<RenderModel(vk::CommandBuffer)>;

    auto update(
        const gfx::Camera&               camera,
        const renderer::base::Allocator& allocator,
        vk::Queue                        sparse_queue,
        vk::CommandBuffer                transfer_command_buffer
    ) -> void;

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

        std::vector<Primitive> primitives;
    };

    std::optional<renderer::resources::Buffer> m_index_buffer;

    // Base descriptor set
    std::optional<renderer::resources::Buffer>                 m_vertex_buffer;
    renderer::resources::RandomAccessBuffer<vk::DeviceAddress> m_vertex_uniform;

    std::optional<renderer::resources::Buffer>                 m_transform_buffer;
    renderer::resources::RandomAccessBuffer<vk::DeviceAddress> m_transform_uniform;

    vk::UniqueSampler m_default_sampler;

    std::optional<renderer::resources::Buffer>                 m_texture_buffer;
    renderer::resources::RandomAccessBuffer<vk::DeviceAddress> m_texture_uniform;

    renderer::resources::RandomAccessBuffer<ShaderMaterial> m_default_material_uniform;

    std::optional<renderer::resources::Buffer>                 m_material_buffer;
    renderer::resources::RandomAccessBuffer<vk::DeviceAddress> m_material_uniform;

    vk::UniqueDescriptorSet m_base_descriptor_set;

    // Image descriptor set
    using ImageVariantLoader =
        std::variant<gfx::resources::Image::Loader, gfx::resources::VirtualImage::Loader>;
    using ImageVariant = std::variant<gfx::resources::Image, gfx::resources::VirtualImage>;
    std::vector<ImageVariant> m_images;
    vk::UniqueDescriptorSet   m_image_descriptor_set;

    // Sampler descriptor set
    std::vector<vk::UniqueSampler> m_samplers;
    vk::UniqueDescriptorSet        m_sampler_descriptor_set;

    // Pipelines
    std::vector<Mesh> m_meshes;

    using UpdateVirtualImagesT = std::function<
        void(std::span<ImageVariant>, const gfx::Camera&, uint32_t, std::optional<uint32_t>)>;
    UpdateVirtualImagesT m_update_virtual_images;


    explicit RenderModel(
        vk::Device                                                   device,
        std::optional<renderer::resources::Buffer>&&                 index_buffer,
        std::optional<renderer::resources::Buffer>&&                 vertex_buffer,
        renderer::resources::RandomAccessBuffer<vk::DeviceAddress>&& vertex_uniform,
        std::optional<renderer::resources::Buffer>&&                 transform_buffer,
        renderer::resources::RandomAccessBuffer<vk::DeviceAddress>&& transform_uniform,
        vk::UniqueSampler&&                                          default_sampler,
        std::optional<renderer::resources::Buffer>&&                 texture_buffer,
        renderer::resources::RandomAccessBuffer<vk::DeviceAddress>&& texture_uniform,
        renderer::resources::RandomAccessBuffer<ShaderMaterial>&& default_material_uniform,
        std::optional<renderer::resources::Buffer>&&                 material_buffer,
        renderer::resources::RandomAccessBuffer<vk::DeviceAddress>&& material_uniform,
        vk::UniqueDescriptorSet&&                                    base_descriptor_set,
        std::vector<ImageVariant>&&                                  images,
        vk::UniqueDescriptorSet&&                                    image_descriptor_set,
        std::vector<vk::UniqueSampler>&&                             samplers,
        vk::UniqueDescriptorSet&& sampler_descriptor_set,
        std::vector<Mesh>&&       meshes,
        UpdateVirtualImagesT&&    update_virtual_images
    );
};

}   // namespace core::gltf
