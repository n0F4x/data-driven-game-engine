module;

#include <vulkan/vulkan.hpp>

export module demos.virtual_texture.init;

import modules.image.Image;
import modules.gfx.resources.VirtualImage;
import modules.renderer.base.allocator.Allocator;
import modules.renderer.base.device.Device;
import modules.renderer.resources.Buffer;
import modules.renderer.resources.Image;
import modules.renderer.resources.RandomAccessBuffer;
import modules.renderer.scene.Scene;

import demos.virtual_texture.Camera;
import demos.virtual_texture.VirtualTexture;
import demos.virtual_texture.VirtualTextureInfo;

namespace demo::init {

export [[nodiscard]]
auto create_descriptor_set_layout(vk::Device device) -> vk::UniqueDescriptorSetLayout;

export [[nodiscard]]
auto create_pipeline_layout(
    vk::Device                               device,
    std::span<const vk::DescriptorSetLayout> descriptor_set_layouts
) -> vk::UniquePipelineLayout;

export [[nodiscard]]
auto create_depth_image(
    vk::PhysicalDevice                     physical_device,
    const modules::renderer::base::Allocator& allocator,
    vk::Extent2D                           swapchain_extent
) -> modules::renderer::resources::Image;

export [[nodiscard]]
auto create_depth_image_view(
    const modules::renderer::base::Device& device,
    vk::Image                           depth_image
) -> vk::UniqueImageView;

export [[nodiscard]]
auto create_pipeline(
    vk::Device         device,
    vk::PipelineLayout layout,
    vk::Format         surface_format,
    vk::Format         depth_format,
    std::string_view   fragment_shader_file_name
) -> vk::UniquePipeline;

export [[nodiscard]]
auto create_camera_buffer(const modules::renderer::base::Allocator& allocator)
    -> modules::renderer::resources::RandomAccessBuffer<Camera>;

export [[nodiscard]]
auto create_virtual_image(
    const modules::renderer::base::Device&    device,
    const modules::renderer::base::Allocator& allocator,
    std::unique_ptr<modules::image::Image>&&  source
) -> modules::gfx::resources::VirtualImage;

export [[nodiscard]]
auto create_virtual_image_sampler(const modules::renderer::base::Device& device)
    -> vk::UniqueSampler;

export [[nodiscard]]
auto create_virtual_texture_info_buffer(
    const modules::renderer::base::Allocator&    allocator,
    const modules::gfx::resources::VirtualImage& virtual_image
) -> modules::renderer::resources::RandomAccessBuffer<VirtualTextureInfo>;

export [[nodiscard]]
auto create_virtual_texture_blocks_buffer(
    const modules::renderer::base::Allocator&    allocator,
    const modules::gfx::resources::VirtualImage& virtual_image
) -> modules::renderer::resources::Buffer;

export [[nodiscard]]
auto create_virtual_texture_blocks_uniform(
    vk::Device                             device,
    const modules::renderer::base::Allocator& allocator,
    vk::Buffer                             virtual_blocks_buffer
) -> modules::renderer::resources::RandomAccessBuffer<vk::DeviceAddress>;

export [[nodiscard]]
auto create_debug_texture_descriptor_set(
    vk::Device              device,
    vk::DescriptorSetLayout descriptor_set_layout,
    vk::DescriptorPool      descriptor_pool,
    vk::Buffer              camera_buffer,
    vk::ImageView           image_view,
    vk::Sampler             sampler
) -> vk::UniqueDescriptorSet;

export [[nodiscard]]
auto create_virtual_texture_descriptor_set(
    vk::Device              device,
    vk::DescriptorSetLayout descriptor_set_layout,
    vk::DescriptorPool      descriptor_pool,
    vk::Buffer              camera_buffer,
    const VirtualTexture&   virtual_texture,
    vk::Buffer              virtual_texture_info_buffer,
    vk::Buffer              virtual_texture_blocks_uniform
) -> vk::UniqueDescriptorSet;

}   // namespace demo::init
