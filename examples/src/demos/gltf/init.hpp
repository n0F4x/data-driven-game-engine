#pragma once

#include <vector>

#include <vulkan/vulkan.hpp>

#include <core/renderer/base/allocator/Allocator.hpp>
#include <core/renderer/resources/Image.hpp>

namespace demo::init {

[[nodiscard]]
auto create_render_pass(vk::Format color_format, const core::renderer::base::Device& device)
    -> vk::UniqueRenderPass;

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
auto create_framebuffers(
    vk::Device                           device,
    vk::Extent2D                         swapchain_extent,
    std::span<const vk::UniqueImageView> swapchain_image_views,
    vk::RenderPass                       render_pass,
    vk::ImageView                        depth_image_view
) -> std::vector<vk::UniqueFramebuffer>;

}   // namespace init