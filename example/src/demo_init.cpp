#include "demo_init.hpp"

#include <print>

#include <vulkan/vulkan.hpp>

#include <core/renderer/base/device/Device.hpp>

using namespace core;

namespace {

[[nodiscard]]
auto find_supported_format(
    const vk::PhysicalDevice       t_physical_device,
    const std::vector<vk::Format>& t_candidates,
    const vk::ImageTiling          t_tiling,
    const vk::FormatFeatureFlags   t_features
) -> vk::Format
{
    for (const auto format : t_candidates) {
        vk::FormatProperties format_properties;
        t_physical_device.getFormatProperties(format, &format_properties);

        if ((t_tiling == vk::ImageTiling::eLinear
             && (format_properties.linearTilingFeatures & t_features) == t_features)
            || (t_tiling == vk::ImageTiling::eOptimal
                && (format_properties.optimalTilingFeatures & t_features) == t_features))
        {
            return format;
        }
    }
    throw std::runtime_error{ "Failed to find supported format!" };
}

[[nodiscard]]
auto find_depth_format(const vk::PhysicalDevice t_physical_device) noexcept -> vk::Format
{
    using enum vk::Format;
    return find_supported_format(
        t_physical_device,
        { eD32Sfloat, eD32SfloatS8Uint, eD24UnormS8Uint },
        vk::ImageTiling::eOptimal,
        vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

}   // namespace

namespace init {

auto create_render_pass(
    const vk::SurfaceFormatKHR& t_surface_format,
    const renderer::Device&     t_device
) -> vk::UniqueRenderPass
{
    const vk::AttachmentDescription color_attachment_description{
        .format         = t_surface_format.format,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .finalLayout    = vk::ImageLayout::ePresentSrcKHR,
    };
    vk::AttachmentReference color_attachment_reference{
        .attachment = 0,
        .layout     = vk::ImageLayout::eColorAttachmentOptimal,
    };

    const vk::AttachmentDescription depth_attachment_description{
        .format         = find_depth_format(t_device.physical_device()),
        .samples        = vk::SampleCountFlagBits::e1,
        .loadOp         = vk::AttachmentLoadOp::eClear,
        .storeOp        = vk::AttachmentStoreOp::eDontCare,
        .stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout  = vk::ImageLayout::eUndefined,
        .finalLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };
    vk::AttachmentReference depth_attachment_reference{
        .attachment = 1,
        .layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal,
    };

    std::array attachment_descriptions{ color_attachment_description,
                                        depth_attachment_description };

    vk::SubpassDescription subpass_description{
        .pipelineBindPoint       = vk::PipelineBindPoint::eGraphics,
        .colorAttachmentCount    = 1,
        .pColorAttachments       = &color_attachment_reference,
        .pDepthStencilAttachment = &depth_attachment_reference,
    };

    vk::SubpassDependency subpass_dependency{
        .srcSubpass   = VK_SUBPASS_EXTERNAL,
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

    return t_device->createRenderPassUnique(render_pass_create_info);
}

auto create_depth_image(
    const vk::PhysicalDevice t_physical_device,
    VmaAllocator             t_allocator,
    const vk::Extent2D       t_swapchain_extent
) noexcept -> renderer::Image
{
    const vk::ImageCreateInfo image_create_info = {
        .imageType = vk::ImageType::e2D,
        .format    = find_depth_format(t_physical_device),
        .extent = vk::Extent3D{ t_swapchain_extent.width, t_swapchain_extent.height, 1 },
        .mipLevels     = 1,
        .arrayLayers   = 1,
        .samples       = vk::SampleCountFlagBits::e1,
        .tiling        = vk::ImageTiling::eOptimal,
        .usage         = vk::ImageUsageFlagBits::eDepthStencilAttachment,
        .initialLayout = vk::ImageLayout::eUndefined,
    };

    constexpr VmaAllocationCreateInfo allocation_create_info = {
        .flags    = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        .usage    = VMA_MEMORY_USAGE_AUTO,
        .priority = 1.f,
    };

    vk::Image     image{};
    VmaAllocation allocation;
    vmaCreateImage(
        t_allocator,
        reinterpret_cast<const VkImageCreateInfo*>(&image_create_info),
        &allocation_create_info,
        reinterpret_cast<VkImage*>(&image),
        &allocation,
        nullptr
    );
    return renderer::Image(image, allocation, t_allocator);
}

auto create_depth_image_view(
    const renderer::Device& t_device,
    const vk::Image         t_depth_image
) -> vk::UniqueImageView
{
    const vk::ImageViewCreateInfo image_view_create_info{
        .image    = t_depth_image,
        .viewType = vk::ImageViewType::e2D,
        .format   = find_depth_format(t_device.physical_device()),
        .subresourceRange =
            vk::ImageSubresourceRange{ .aspectMask     = vk::ImageAspectFlagBits::eDepth,
                                      .baseMipLevel   = 0,
                                      .levelCount     = 1,
                                      .baseArrayLayer = 0,
                                      .layerCount     = 1 },
    };

    return t_device->createImageViewUnique(image_view_create_info);
}

auto create_framebuffers(
    const vk::Device                        t_device,
    const vk::Extent2D                      t_swapchain_extent,
    const std::vector<vk::UniqueImageView>& t_swapchain_image_views,
    const vk::RenderPass                    t_render_pass,
    const vk::ImageView                     t_depth_image_view
) -> std::vector<vk::UniqueFramebuffer>
{
    std::vector<vk::UniqueFramebuffer> framebuffers;
    framebuffers.reserve(t_swapchain_image_views.size());

    for (const auto& swapchain_image_view : t_swapchain_image_views) {
        std::array attachments{ *swapchain_image_view, t_depth_image_view };

        vk::FramebufferCreateInfo framebuffer_create_info{
            .renderPass      = t_render_pass,
            .attachmentCount = static_cast<uint32_t>(attachments.size()),
            .pAttachments    = attachments.data(),
            .width           = t_swapchain_extent.width,
            .height          = t_swapchain_extent.height,
            .layers          = 1
        };

        framebuffers.emplace_back(t_device.createFramebufferUnique(framebuffer_create_info
        ));
    }

    return framebuffers;
}

auto create_command_pool(const vk::Device t_device, const uint32_t t_queue_family_index)
    -> vk::UniqueCommandPool
{
    const vk::CommandPoolCreateInfo command_pool_create_info{
        .flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = t_queue_family_index
    };

    return t_device.createCommandPoolUnique(command_pool_create_info);
}

auto create_command_buffers(
    const vk::Device      t_device,
    const vk::CommandPool t_command_pool,
    const uint32_t        t_count
) -> std::vector<vk::CommandBuffer>
{
    const vk::CommandBufferAllocateInfo command_buffer_allocate_info{
        .commandPool        = t_command_pool,
        .level              = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = t_count
    };

    return t_device.allocateCommandBuffers(command_buffer_allocate_info);
}

auto create_semaphores(const vk::Device t_device, const uint32_t t_count)
    -> std::vector<vk::UniqueSemaphore>
{
    constexpr vk::SemaphoreCreateInfo createInfo{};
    std::vector<vk::UniqueSemaphore>  semaphores;
    semaphores.reserve(t_count);

    for (uint32_t i = 0; i < t_count; i++) {
        semaphores.emplace_back(t_device.createSemaphoreUnique(createInfo));
    }

    return semaphores;
}

auto create_fences(const vk::Device t_device, const uint32_t t_count)
    -> std::vector<vk::UniqueFence>
{
    constexpr vk::FenceCreateInfo fence_create_info{
        .flags = vk::FenceCreateFlagBits::eSignaled
    };
    std::vector<vk::UniqueFence> fences;
    fences.reserve(t_count);

    for (uint32_t i = 0; i < t_count; i++) {
        fences.emplace_back(t_device.createFenceUnique(fence_create_info));
    }

    return fences;
}

}   // namespace init
