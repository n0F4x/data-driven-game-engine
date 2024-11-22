module;

#include <limits>
#include <ranges>
#include <set>
#include <utility>

#include <spdlog/spdlog.h>

#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_to_string.hpp>

module core.renderer.base.swapchain.Swapchain;

namespace core::renderer::base {

[[nodiscard]]
static auto get_min_image_count(const vk::SurfaceCapabilitiesKHR& surface_capabilities
) noexcept -> uint32_t
{
    uint32_t image_count = surface_capabilities.minImageCount + 1;
    if (surface_capabilities.maxImageCount > 0
        && image_count > surface_capabilities.maxImageCount)
    {
        image_count = surface_capabilities.maxImageCount;
    }
    return image_count;
}

[[nodiscard]]
static auto choose_swapchain_surface_format(
    const vk::SurfaceKHR     surface,
    const vk::PhysicalDevice physical_device
) -> vk::SurfaceFormatKHR
{
    const auto available_surface_formats{ physical_device.getSurfaceFormatsKHR(surface) };

    for (const auto& surface_format : available_surface_formats) {
        if (surface_format.format == vk::Format::eB8G8R8A8Srgb
            && surface_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return surface_format;
        }
    }

    return available_surface_formats.front();
}

[[nodiscard]]
static auto choose_swapchain_present_mode(
    const vk::SurfaceKHR     surface,
    const vk::PhysicalDevice physical_device
) -> vk::PresentModeKHR
{
    const auto present_modes{ physical_device.getSurfacePresentModesKHR(surface) };
    return std::ranges::find(present_modes, vk::PresentModeKHR::eMailbox)
                != std::cend(present_modes)
             ? vk::PresentModeKHR::eMailbox
             : vk::PresentModeKHR::eFifo;
}

[[nodiscard]]
static auto create_swapchain(
    const vk::SurfaceKHR              surface,
    const vk::PhysicalDevice          physical_device,
    const uint32_t                    graphics_queue_family,
    const uint32_t                    present_queue_family,
    const vk::Device                  device,
    const vk::SurfaceCapabilitiesKHR& surface_capabilities,
    const vk::Extent2D                extent,
    const vk::SurfaceFormatKHR        surface_format,
    const vk::SwapchainKHR            old_swapchain
) -> vk::UniqueSwapchainKHR
{
    const std::set              buffer{ graphics_queue_family, present_queue_family };
    const std::vector<uint32_t> queue_family_indices = { buffer.cbegin(), buffer.cend() };
    const vk::SharingMode       sharing_mode         = queue_family_indices.size() > 1
                                                         ? vk::SharingMode::eConcurrent
                                                         : vk::SharingMode::eExclusive;

    const vk::SwapchainCreateInfoKHR create_info{
        .surface               = surface,
        .minImageCount         = get_min_image_count(surface_capabilities),
        .imageFormat           = surface_format.format,
        .imageColorSpace       = surface_format.colorSpace,
        .imageExtent           = extent,
        .imageArrayLayers      = 1,
        .imageUsage            = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode      = sharing_mode,
        .queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices.size()),
        .pQueueFamilyIndices   = queue_family_indices.data(),
        .preTransform          = surface_capabilities.currentTransform,
        .compositeAlpha        = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode           = choose_swapchain_present_mode(surface, physical_device),
        .clipped               = vk::True,
        .oldSwapchain          = old_swapchain
    };

    return device.createSwapchainKHRUnique(create_info);
}

[[nodiscard]]
static auto create_image_views(
    const vk::Device       device,
    const vk::SwapchainKHR swapchain,
    const vk::Format       format
) -> std::vector<vk::UniqueImageView>
{
    return device.getSwapchainImagesKHR(swapchain)
         | std::views::transform([device, format](const vk::Image image) {
               const vk::ImageViewCreateInfo image_view_create_info{
                   .image    = image,
                   .viewType = vk::ImageViewType::e2D,
                   .format   = format,
                   .subresourceRange =
                       vk::ImageSubresourceRange{
                                                 .aspectMask     = vk::ImageAspectFlagBits::eColor,
                                                 .baseMipLevel   = 0,
                                                 .levelCount     = 1,
                                                 .baseArrayLayer = 0,
                                                 .layerCount     = 1 },
               };
               return device.createImageViewUnique(image_view_create_info);
           })
         | std::ranges::to<std::vector>();
}

auto Swapchain::choose_extent(
    const vk::Extent2D&               framebuffer_size,
    const vk::SurfaceCapabilitiesKHR& surface_capabilities
) noexcept -> vk::Extent2D
{
    if (surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return surface_capabilities.currentExtent;
    }

    return vk::Extent2D{
        .width = std::clamp(
            static_cast<uint32_t>(framebuffer_size.width),
            surface_capabilities.minImageExtent.width,
            surface_capabilities.maxImageExtent.width
        ),
        .height = std::clamp(
            static_cast<uint32_t>(framebuffer_size.height),
            surface_capabilities.minImageExtent.height,
            surface_capabilities.maxImageExtent.height
        ),
    };
}

auto Swapchain::create(
    const vk::SurfaceKHR     surface,
    const vk::PhysicalDevice physical_device,
    const uint32_t           graphics_queue_family_index,
    const uint32_t           present_queue_family_index,
    const vk::Device         device,
    const vk::Extent2D       framebuffer_size,
    const vk::SwapchainKHR   old_swapchain
) -> std::optional<Swapchain>
{
    const vk::SurfaceCapabilitiesKHR surface_capabilities{
        physical_device.getSurfaceCapabilitiesKHR(surface)
    };

    const vk::Extent2D extent = choose_extent(framebuffer_size, surface_capabilities);
    if (extent.width == 0 || extent.height == 0) {
        return std::nullopt;
    }

    const vk::SurfaceFormatKHR surface_format{
        choose_swapchain_surface_format(surface, physical_device)
    };

    vk::UniqueSwapchainKHR swapchain{ create_swapchain(
        surface,
        physical_device,
        graphics_queue_family_index,
        present_queue_family_index,
        device,
        surface_capabilities,
        extent,
        surface_format,
        old_swapchain
    ) };

    return Swapchain{ device,
                      extent,
                      surface_format.format,
                      std::move(swapchain),
                      create_image_views(device, swapchain.get(), surface_format.format) };
}

auto Swapchain::get() const noexcept -> vk::SwapchainKHR
{
    return m_swapchain.operator*();
}

auto Swapchain::extent() const noexcept -> vk::Extent2D
{
    return m_extent;
}

auto Swapchain::format() const noexcept -> vk::Format
{
    return m_format;
}

auto Swapchain::images() const noexcept -> std::span<const vk::Image>
{
    return m_images;
}

auto Swapchain::image_views() const noexcept -> std::span<const vk::UniqueImageView>
{
    return m_image_views;
}

Swapchain::Swapchain(
    const vk::Device                   device,
    const vk::Extent2D                 extent,
    const vk::Format                   format,
    vk::UniqueSwapchainKHR&&           swapchain,
    std::vector<vk::UniqueImageView>&& image_views
) noexcept
    : m_device{ device },
      m_extent{ extent },
      m_format{ format },
      m_swapchain{ std::move(swapchain) },
      m_images{ device.getSwapchainImagesKHR(m_swapchain.get()) },
      m_image_views{ std::move(image_views) }
{}

}   // namespace core::renderer::base
