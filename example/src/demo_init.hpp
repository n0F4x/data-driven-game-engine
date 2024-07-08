#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

#include <core/renderer/base/allocator/Allocator.hpp>
#include <core/renderer/memory/Image.hpp>

namespace init {

[[nodiscard]]
auto create_render_pass(vk::Format t_color_format, const core::renderer::Device& t_device)
    -> vk::UniqueRenderPass;

[[nodiscard]]
auto create_depth_image(
    vk::PhysicalDevice               physical_device,
    const core::renderer::Allocator& allocator,
    vk::Extent2D                     swapchain_extent
) -> core::renderer::Image;

[[nodiscard]]
auto create_depth_image_view(
    const core::renderer::Device& t_device,
    vk::Image                     t_depth_image
) -> vk::UniqueImageView;

[[nodiscard]]
auto create_framebuffers(
    vk::Device                              t_device,
    vk::Extent2D                            t_swapchain_extent,
    const std::vector<vk::UniqueImageView>& t_swapchain_image_views,
    vk::RenderPass                          t_render_pass,
    vk::ImageView                           t_depth_image_view
) -> std::vector<vk::UniqueFramebuffer>;

[[nodiscard]]
auto create_command_pool(vk::Device t_device, uint32_t t_queue_family_index)
    -> vk::UniqueCommandPool;

[[nodiscard]]
auto create_command_buffers(
    vk::Device      t_device,
    vk::CommandPool t_command_pool,
    uint32_t        t_count
) -> std::vector<vk::CommandBuffer>;

[[nodiscard]]
auto create_semaphores(vk::Device t_device, uint32_t t_count)
    -> std::vector<vk::UniqueSemaphore>;

[[nodiscard]]
auto create_fences(vk::Device t_device, uint32_t t_count) -> std::vector<vk::UniqueFence>;

}   // namespace init
