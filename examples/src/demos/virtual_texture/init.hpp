#pragma once

#include <vulkan/vulkan.hpp>

#include <core/gfx/resources/VirtualImage.hpp>
#include <core/renderer/resources/Image.hpp>
#include <core/renderer/resources/RandomAccessBuffer.hpp>
#include <core/renderer/scene/Builder.hpp>

#include "Camera.hpp"
#include "VirtualTextureInfo.hpp"

namespace demo {
class VirtualTexture;
}   // namespace demo

namespace demo::init {

[[nodiscard]]
auto create_descriptor_set_layout(vk::Device device) -> vk::UniqueDescriptorSetLayout;

[[nodiscard]]
auto create_pipeline_layout(
    vk::Device                               device,
    std::span<const vk::DescriptorSetLayout> descriptor_set_layouts
) -> vk::UniquePipelineLayout;

[[nodiscard]]
auto create_depth_image(
    vk::PhysicalDevice                     physical_device,
    const core::renderer::base::Allocator& allocator,
    vk::Extent2D                           swapchain_extent
) -> core::renderer::resources::Image;

[[nodiscard]]
auto create_depth_image_view(
    const core::renderer::base::Device& device,
    vk::Image                           depth_image
) -> vk::UniqueImageView;

[[nodiscard]]
auto create_pipeline(
    vk::Device         device,
    vk::PipelineLayout layout,
    vk::Format         surface_format,
    vk::Format         depth_format,
    std::string_view   fragment_shader_file_name
) -> vk::UniquePipeline;

[[nodiscard]]
auto create_camera_buffer(const core::renderer::base::Allocator& allocator)
    -> core::renderer::resources::RandomAccessBuffer<Camera>;

[[nodiscard]]
auto create_virtual_image(
    const core::renderer::base::Device&    device,
    const core::renderer::base::Allocator& allocator,
    std::unique_ptr<core::image::Image>&&  source
) -> core::gfx::resources::VirtualImage;

[[nodiscard]]
auto create_virtual_image_sampler(const core::renderer::base::Device& device)
    -> vk::UniqueSampler;

[[nodiscard]]
auto create_virtual_texture_info_buffer(
    const core::renderer::base::Allocator&    allocator,
    const core::gfx::resources::VirtualImage& virtual_image
) -> core::renderer::resources::RandomAccessBuffer<VirtualTextureInfo>;

[[nodiscard]]
auto create_virtual_texture_blocks_buffer(
    const core::renderer::base::Allocator&    allocator,
    const core::gfx::resources::VirtualImage& virtual_image
) -> core::renderer::resources::Buffer;

[[nodiscard]]
auto create_virtual_texture_blocks_uniform(
    vk::Device                             device,
    const core::renderer::base::Allocator& allocator,
    vk::Buffer                             virtual_blocks_buffer
) -> core::renderer::resources::RandomAccessBuffer<vk::DeviceAddress>;

[[nodiscard]]
auto create_debug_texture_descriptor_set(
    vk::Device              device,
    vk::DescriptorSetLayout descriptor_set_layout,
    vk::DescriptorPool      descriptor_pool,
    vk::Buffer              camera_buffer,
    vk::ImageView           image_view,
    vk::Sampler             sampler
) -> vk::UniqueDescriptorSet;

[[nodiscard]]
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
