module;

#include <vector>

#include <vulkan/vulkan.hpp>

export module demos.gltf.init;

import core.renderer.base.device.Device;
import core.renderer.base.allocator.Allocator;
import core.renderer.resources.Image;

namespace demo::init {

export [[nodiscard]]
auto create_render_pass(vk::Format color_format, const core::renderer::base::Device& device)
    -> vk::UniqueRenderPass;

export [[nodiscard]]
auto create_depth_image(
    vk::PhysicalDevice                     physical_device,
    const core::renderer::base::Allocator& allocator,
    vk::Extent2D                           swapchain_extent
) -> core::renderer::resources::Image;

export [[nodiscard]]
auto create_depth_image_view(
    const core::renderer::base::Device& device,
    vk::Image                           depth_image
) -> vk::UniqueImageView;

export [[nodiscard]]
auto create_framebuffers(
    vk::Device                           device,
    vk::Extent2D                         swapchain_extent,
    std::span<const vk::UniqueImageView> swapchain_image_views,
    vk::RenderPass                       render_pass,
    vk::ImageView                        depth_image_view
) -> std::vector<vk::UniqueFramebuffer>;

}   // namespace demo::init
