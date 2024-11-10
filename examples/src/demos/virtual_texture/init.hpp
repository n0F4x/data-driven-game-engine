#pragma once

#include <vulkan/vulkan.hpp>

#include <core/gfx/resources/VirtualImage.hpp>
#include <core/renderer/resources/Image.hpp>
#include <core/renderer/resources/RandomAccessBuffer.hpp>

#include "Camera.hpp"

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
    vk::Format         depth_format
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
auto create_descriptor_set(
    vk::Device              device,
    vk::DescriptorSetLayout descriptor_set_layout,
    vk::DescriptorPool      descriptor_pool,
    vk::Buffer              camera_buffer,
    vk::ImageView           virtual_texture_image_view,
    vk::Sampler             virtual_texture_image_sampler
) -> vk::UniqueDescriptorSet;

}   // namespace demo::init
