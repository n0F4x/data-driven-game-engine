module;

#include <vulkan/vulkan.hpp>

#include <vk_mem_alloc.h>

module demos.gltf.init;

import modules.renderer.base.device.Device;
import modules.renderer.resources.Image;

[[nodiscard]]
auto find_supported_format(
    const vk::PhysicalDevice          physical_device,
    const std::span<const vk::Format> candidates,
    const vk::ImageTiling             tiling,
    const vk::FormatFeatureFlags      features
) -> vk::Format
{
    for (const auto format : candidates) {
        vk::FormatProperties format_properties;
        physical_device.getFormatProperties(format, &format_properties);

        if ((tiling == vk::ImageTiling::eLinear
             && (format_properties.linearTilingFeatures & features) == features)
            || (tiling == vk::ImageTiling::eOptimal
                && (format_properties.optimalTilingFeatures & features) == features))
        {
            return format;
        }
    }
    throw std::runtime_error{ "Failed to find supported format!" };
}

[[nodiscard]]
auto find_depth_format(const vk::PhysicalDevice physical_device) -> vk::Format
{
    using enum vk::Format;
    return ::find_supported_format(
        physical_device,
        std::array{ eD32Sfloat, eD32SfloatS8Uint, eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

auto demo::init::create_render_pass(
    const vk::Format                    color_format,
    const modules::renderer::base::Device& device
) -> vk::UniqueRenderPass
{
    const vk::AttachmentDescription color_attachment_description{
        .format         = color_format,
        .samples        = vk::SampleCountFlagBits::e1,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
    };
    constexpr static vk::AttachmentReference color_attachment_reference{
        .attachment = 0,
        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
    };

    const vk::AttachmentDescription depth_attachment_description{
        .format         = ::find_depth_format(device.physical_device()),
        .samples        = vk::SampleCountFlagBits::e1,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };
    constexpr static vk::AttachmentReference depth_attachment_reference{
        .attachment = 1,
        .layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    const std::array attachment_descriptions{
        color_attachment_description,
        depth_attachment_description,
    };

    constexpr vk::SubpassDescription subpass_description{
        .pipelineBindPoint       = vk::PipelineBindPoint::eGraphics,
        .colorAttachmentCount    = 1,
        .pColorAttachments       = &color_attachment_reference,
        .pDepthStencilAttachment = &depth_attachment_reference,
    };

    constexpr static vk::SubpassDependency subpass_dependency{
        .srcSubpass   = vk::SubpassExternal,
        .dstSubpass   = 0,
        .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput
                      | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput
                      | vk::PipelineStageFlagBits::eEarlyFragmentTests,
        .srcAccessMask = vk::AccessFlagBits::eNone,
        .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
                       | vk::AccessFlagBits::eDepthStencilAttachmentWrite,
    };

    const vk::RenderPassCreateInfo render_pass_create_info{
        .attachmentCount = static_cast<uint32_t>(attachment_descriptions.size()),
        .pAttachments    = attachment_descriptions.data(),
        .subpassCount    = 1,
        .pSubpasses      = &subpass_description,
        .dependencyCount = 1,
        .pDependencies   = &subpass_dependency,
    };

    return device->createRenderPassUnique(render_pass_create_info);
}

auto demo::init::create_depth_image(
    const vk::PhysicalDevice               physical_device,
    const modules::renderer::base::Allocator& allocator,
    const vk::Extent2D                     swapchain_extent
) -> modules::renderer::resources::Image
{
    const vk::ImageCreateInfo image_create_info = {
        .imageType   = vk::ImageType::e2D,
        .format      = ::find_depth_format(physical_device),
        .extent      = vk::Extent3D{ swapchain_extent.width, swapchain_extent.height, 1 },
        .mipLevels   = 1,
        .arrayLayers = 1,
        .samples     = vk::SampleCountFlagBits::e1,
        .tiling      = vk::ImageTiling::eOptimal,
        .usage       = vk::ImageUsageFlagBits::eDepthStencilAttachment,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    constexpr static VmaAllocationCreateInfo allocation_create_info = {
        .flags    = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        .usage    = VMA_MEMORY_USAGE_AUTO,
        .priority = 1.f,
    };

    return modules::renderer::resources::Image{ allocator,
                                             image_create_info,
                                             allocation_create_info };
}

auto demo::init::create_depth_image_view(
    const modules::renderer::base::Device& device,
    const vk::Image                     depth_image
) -> vk::UniqueImageView
{
    const vk::ImageViewCreateInfo image_view_create_info{
        .image    = depth_image,
        .viewType = vk::ImageViewType::e2D,
        .format   = ::find_depth_format(device.physical_device()),
        .subresourceRange =
            vk::ImageSubresourceRange{ .aspectMask     = vk::ImageAspectFlagBits::eDepth,
                                      .baseMipLevel   = 0,
                                      .levelCount     = 1,
                                      .baseArrayLayer = 0,
                                      .layerCount     = 1 },
    };

    return device->createImageViewUnique(image_view_create_info);
}

auto demo::init::create_framebuffers(
    const vk::Device                           device,
    const vk::Extent2D                         swapchain_extent,
    const std::span<const vk::UniqueImageView> swapchain_image_views,
    const vk::RenderPass                       render_pass,
    const vk::ImageView                        depth_image_view
) -> std::vector<vk::UniqueFramebuffer>
{
    std::vector<vk::UniqueFramebuffer> framebuffers;
    framebuffers.reserve(swapchain_image_views.size());

    for (const vk::UniqueImageView& swapchain_image_view : swapchain_image_views) {
        std::array attachments{ swapchain_image_view.get(), depth_image_view };

        const vk::FramebufferCreateInfo framebuffer_create_info{
            .renderPass      = render_pass,
            .attachmentCount = static_cast<uint32_t>(attachments.size()),
            .pAttachments    = attachments.data(),
            .width           = swapchain_extent.width,
            .height          = swapchain_extent.height,
            .layers          = 1
        };

        framebuffers.emplace_back(device.createFramebufferUnique(framebuffer_create_info));
    }

    return framebuffers;
}
