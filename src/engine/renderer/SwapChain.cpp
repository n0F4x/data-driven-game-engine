#include "SwapChain.hpp"

#include <algorithm>   // std::clamp
#include <limits>      // std::numeric_limits
#include <set>

namespace engine::renderer {

namespace {

vk::SurfaceFormatKHR choose_swap_chain_surface_format(
    const std::vector<vk::SurfaceFormatKHR>& t_available_surface_formats
)
{
    for (const auto& surface_format : t_available_surface_formats) {
        if (surface_format.format == vk::Format::eB8G8R8A8Srgb
            && surface_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return surface_format;
        }
    }
    return t_available_surface_formats.front();
}

vk::PresentModeKHR choose_swap_chain_present_mode(
    const std::vector<vk::PresentModeKHR>& t_available_present_modes
)
{
    for (auto available_present_mode : t_available_present_modes) {
        if (available_present_mode == vk::PresentModeKHR::eMailbox) {
            return available_present_mode;
        }
    }
    return vk::PresentModeKHR::eFifo;
}

vk::Extent2D choose_swap_chain_extent(
    const vk::Extent2D&               t_frame_buffer_size,
    const vk::SurfaceCapabilitiesKHR& t_surface_capabilities
)
{
    if (t_surface_capabilities.currentExtent.width
        == std::numeric_limits<uint32_t>::max())
    {
        auto actual_extent = t_frame_buffer_size;

        actual_extent.width = std::clamp(
            actual_extent.width,
            t_surface_capabilities.minImageExtent.width,
            t_surface_capabilities.maxImageExtent.width
        );
        actual_extent.height = std::clamp(
            actual_extent.height,
            t_surface_capabilities.minImageExtent.height,
            t_surface_capabilities.maxImageExtent.height
        );

        return { actual_extent.width, actual_extent.height };
    }
    else {
        return t_surface_capabilities.currentExtent;
    }
}

vk::raii::SwapchainKHR create_swap_chain(
    const vk::raii::SurfaceKHR&       t_surface,
    const Device&                     t_device,
    const vk::SurfaceCapabilitiesKHR& t_surface_capabilities,
    const vk::Extent2D&               t_extent,
    const vk::SurfaceFormatKHR&       t_surfaceFormat
)
{
    vk::PresentModeKHR present_mode = choose_swap_chain_present_mode(
        t_device.physical_device().getSurfacePresentModesKHR(*t_surface)
    );

    uint32_t image_count = t_surface_capabilities.minImageCount + 1;
    if (t_surface_capabilities.maxImageCount > 0
        && image_count > t_surface_capabilities.maxImageCount)
    {
        image_count = t_surface_capabilities.maxImageCount;
    }

    std::set              buffer{ t_device.graphics_queue_family(),
                     t_device.present_queue_family() };
    std::vector<uint32_t> queueFamilyIndices = { buffer.begin(), buffer.end() };
    vk::SharingMode       sharingMode        = queueFamilyIndices.size() > 1
                                                 ? vk::SharingMode::eConcurrent
                                                 : vk::SharingMode::eExclusive;
    // TODO: implement exclusive sharing for multiple queues

    vk::SwapchainCreateInfoKHR create_info{
        .surface          = *t_surface,
        .minImageCount    = image_count,
        .imageFormat      = t_surfaceFormat.format,
        .imageColorSpace  = t_surfaceFormat.colorSpace,
        .imageExtent      = t_extent,
        .imageArrayLayers = 1,
        .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = sharingMode,
        .queueFamilyIndexCount =
            static_cast<uint32_t>(queueFamilyIndices.size()),
        .pQueueFamilyIndices = queueFamilyIndices.data(),
        .preTransform        = t_surface_capabilities.currentTransform,
        .compositeAlpha      = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode         = present_mode,
        .clipped             = true,
    };

    return { t_device.device(), create_info };
}

std::vector<vk::raii::ImageView> create_image_views(
    const vk::raii::Device&       t_device,
    const vk::raii::SwapchainKHR& t_swap_chain,
    const vk::SurfaceFormatKHR&   t_surface_format
)
{
    auto                             images = t_swap_chain.getImages();
    std::vector<vk::raii::ImageView> image_views;
    image_views.reserve(images.size());

    for (const auto& image : images) {
        image_views.emplace_back(
            t_device,
            vk::ImageViewCreateInfo{
                .image            = image,
                .viewType         = vk::ImageViewType::e2D,
                .format           = t_surface_format.format,
                .subresourceRange = {.aspectMask =
vk::ImageAspectFlagBits::eColor,
                                     .baseMipLevel   = 0,
                                     .levelCount     = 1,
                                     .baseArrayLayer = 0,
                                     .layerCount     = 1}
        }
        );
    }

    return image_views;
}

}   // namespace

////////////////////////////////////
///------------------------------///
///  SwapChain   IMPLEMENTATION  ///
///------------------------------///
////////////////////////////////////
SwapChain::SwapChain(
    const vk::raii::SurfaceKHR&       t_surface,
    const Device&                     t_device,
    const vk::SurfaceCapabilitiesKHR& t_surface_capabilities,
    const vk::Extent2D&               t_extent
)
    : m_extent{ t_extent },
      m_surface_format{ choose_swap_chain_surface_format(
          t_device.physical_device().getSurfaceFormatsKHR(*t_surface)
      ) },
      m_swap_chain{ create_swap_chain(
          t_surface,
          t_device,
          t_surface_capabilities,
          m_extent,
          m_surface_format
      ) },
      m_imageViews{
          create_image_views(t_device.device(), m_swap_chain, m_surface_format)
      }
{}

auto SwapChain::create(
    const vk::raii::SurfaceKHR& t_surface,
    const vk::Extent2D&         t_frame_buffer_size,
    const Device&               t_device
) -> std::optional<SwapChain>
{
    vk::SurfaceCapabilitiesKHR surface_capabilities =
        t_device.physical_device().getSurfaceCapabilitiesKHR(*t_surface);
    auto extent =
        choose_swap_chain_extent(t_frame_buffer_size, surface_capabilities);

    if (extent.width == 0 || extent.height == 0) {
        return std::nullopt;
    }

    return SwapChain{ t_surface, t_device, surface_capabilities, extent };
}

auto SwapChain::extent() const noexcept -> const vk::Extent2D&
{
    return m_extent;
}

auto SwapChain::swap_chain() const noexcept -> const vk::raii::SwapchainKHR&
{
    return m_swap_chain;
}

}   // namespace engine::renderer
