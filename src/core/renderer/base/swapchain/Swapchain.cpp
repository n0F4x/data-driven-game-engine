#include "Swapchain.hpp"

#include <limits>
#include <ranges>
#include <set>
#include <utility>

#include <spdlog/spdlog.h>

#include <vulkan/vulkan_to_string.hpp>

[[nodiscard]]
static auto get_min_image_count(const vk::SurfaceCapabilitiesKHR& t_surface_capabilities
) noexcept -> uint32_t
{
    uint32_t image_count = t_surface_capabilities.minImageCount + 1;
    if (t_surface_capabilities.maxImageCount > 0
        && image_count > t_surface_capabilities.maxImageCount)
    {
        image_count = t_surface_capabilities.maxImageCount;
    }
    return image_count;
}

[[nodiscard]]
static auto choose_swapchain_surface_format(
    const vk::SurfaceKHR     t_surface,
    const vk::PhysicalDevice t_physical_device
) -> vk::SurfaceFormatKHR
{
    const auto available_surface_formats{ t_physical_device.getSurfaceFormatsKHR(t_surface
    ) };

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
    const vk::SurfaceKHR     t_surface,
    const vk::PhysicalDevice t_physical_device
) -> vk::PresentModeKHR
{
    const auto present_modes{ t_physical_device.getSurfacePresentModesKHR(t_surface) };
    return std::ranges::find(present_modes, vk::PresentModeKHR::eMailbox)
                != std::cend(present_modes)
             ? vk::PresentModeKHR::eMailbox
             : vk::PresentModeKHR::eFifo;
}

[[nodiscard]]
static auto create_swapchain(
    const vk::SurfaceKHR              t_surface,
    const vk::PhysicalDevice          t_physical_device,
    const uint32_t                    t_graphics_queue_family,
    const uint32_t                    t_present_queue_family,
    const vk::Device                  t_device,
    const vk::SurfaceCapabilitiesKHR& t_surface_capabilities,
    const vk::Extent2D                t_extent,
    const vk::SurfaceFormatKHR        t_surface_format,
    const vk::SwapchainKHR            t_old_swapchain
) -> vk::UniqueSwapchainKHR
{
    const std::set              buffer{ t_graphics_queue_family, t_present_queue_family };
    const std::vector<uint32_t> queue_family_indices = { buffer.cbegin(), buffer.cend() };
    const vk::SharingMode       sharing_mode         = queue_family_indices.size() > 1
                                                         ? vk::SharingMode::eConcurrent
                                                         : vk::SharingMode::eExclusive;

    const vk::SwapchainCreateInfoKHR create_info{
        .surface               = t_surface,
        .minImageCount         = get_min_image_count(t_surface_capabilities),
        .imageFormat           = t_surface_format.format,
        .imageColorSpace       = t_surface_format.colorSpace,
        .imageExtent           = t_extent,
        .imageArrayLayers      = 1,
        .imageUsage            = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode      = sharing_mode,
        .queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices.size()),
        .pQueueFamilyIndices   = queue_family_indices.data(),
        .preTransform          = t_surface_capabilities.currentTransform,
        .compositeAlpha        = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode  = choose_swapchain_present_mode(t_surface, t_physical_device),
        .clipped      = vk::True,
        .oldSwapchain = t_old_swapchain
    };

    return t_device.createSwapchainKHRUnique(create_info);
}

[[nodiscard]]
static auto create_image_views(
    const vk::Device       t_device,
    const vk::SwapchainKHR t_swapchain,
    const vk::Format       t_format
) -> std::vector<vk::UniqueImageView>
{
    return t_device.getSwapchainImagesKHR(t_swapchain)
         | std::views::transform([t_device, t_format](const vk::Image image) {
               const vk::ImageViewCreateInfo image_view_create_info{
                   .image    = image,
                   .viewType = vk::ImageViewType::e2D,
                   .format   = t_format,
                   .subresourceRange =
                       vk::ImageSubresourceRange{
                                                 .aspectMask     = vk::ImageAspectFlagBits::eColor,
                                                 .baseMipLevel   = 0,
                                                 .levelCount     = 1,
                                                 .baseArrayLayer = 0,
                                                 .layerCount     = 1 },
               };
               return t_device.createImageViewUnique(image_view_create_info);
           })
         | std::ranges::to<std::vector>();
}

namespace core::renderer {

auto Swapchain::choose_extent(
    const vk::Extent2D&               t_framebuffer_size,
    const vk::SurfaceCapabilitiesKHR& t_surface_capabilities
) noexcept -> vk::Extent2D
{
    if (t_surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return t_surface_capabilities.currentExtent;
    }

    return vk::Extent2D{
        .width = std::clamp(
            static_cast<uint32_t>(t_framebuffer_size.width),
            t_surface_capabilities.minImageExtent.width,
            t_surface_capabilities.maxImageExtent.width
        ),
        .height = std::clamp(
            static_cast<uint32_t>(t_framebuffer_size.height),
            t_surface_capabilities.minImageExtent.height,
            t_surface_capabilities.maxImageExtent.height
        ),
    };
}

auto Swapchain::create(
    const vk::SurfaceKHR     t_surface,
    const vk::PhysicalDevice t_physical_device,
    const uint32_t           t_graphics_queue_family_index,
    const uint32_t           t_present_queue_family_index,
    const vk::Device         t_device,
    const vk::Extent2D       t_framebuffer_size,
    const vk::SwapchainKHR   t_old_swapchain
) -> std::optional<Swapchain>
{
    const vk::SurfaceCapabilitiesKHR surface_capabilities{
        t_physical_device.getSurfaceCapabilitiesKHR(t_surface)
    };

    const vk::Extent2D extent = choose_extent(t_framebuffer_size, surface_capabilities);
    if (extent.width == 0 || extent.height == 0) {
        return std::nullopt;
    }

    const vk::SurfaceFormatKHR surface_format{
        choose_swapchain_surface_format(t_surface, t_physical_device)
    };

    vk::UniqueSwapchainKHR swapchain{ create_swapchain(
        t_surface,
        t_physical_device,
        t_graphics_queue_family_index,
        t_present_queue_family_index,
        t_device,
        surface_capabilities,
        extent,
        surface_format,
        t_old_swapchain
    ) };

    return Swapchain{
        t_device,
        extent,
        surface_format.format,
        std::move(swapchain),
        create_image_views(t_device, swapchain.get(), surface_format.format)
    };
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

auto Swapchain::image_views() const noexcept -> const std::vector<vk::UniqueImageView>&
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
      m_image_views{ std::move(image_views) }
{}

}   // namespace core::renderer
