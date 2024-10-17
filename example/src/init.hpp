#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include <core/renderer/base/allocator/Allocator.hpp>
#include <core/renderer/base/memory/Image.hpp>

namespace init {

[[nodiscard]]
auto create_render_pass(vk::Format color_format, const core::renderer::base::Device& device)
    -> vk::UniqueRenderPass;

[[nodiscard]]
auto create_depth_image(
    vk::PhysicalDevice                     physical_device,
    const core::renderer::base::Allocator& allocator,
    vk::Extent2D                           swapchain_extent
) -> core::renderer::base::Image;

[[nodiscard]]
auto create_depth_image_view(
    const core::renderer::base::Device& device,
    vk::Image                           depth_image
) -> vk::UniqueImageView;

[[nodiscard]]
auto create_framebuffers(
    vk::Device                           device,
    vk::Extent2D                         swapchain_extent,
    std::span<const vk::UniqueImageView> swapchain_image_views,
    vk::RenderPass                       render_pass,
    vk::ImageView                        depth_image_view
) -> std::vector<vk::UniqueFramebuffer>;

[[nodiscard]]
auto create_command_pool(vk::Device device, uint32_t queue_family_index)
    -> vk::UniqueCommandPool;

[[nodiscard]]
auto create_command_buffers(vk::Device device, vk::CommandPool command_pool, uint32_t count)
    -> std::vector<vk::CommandBuffer>;

[[nodiscard]]
auto create_semaphores(vk::Device device, uint32_t count)
    -> std::vector<vk::UniqueSemaphore>;

[[nodiscard]]
auto create_fences(vk::Device device, uint32_t count) -> std::vector<vk::UniqueFence>;

}   // namespace init
