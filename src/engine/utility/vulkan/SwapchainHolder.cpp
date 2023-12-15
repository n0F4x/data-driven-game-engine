#include "SwapchainHolder.hpp"

#include <limits>
#include <set>
#include <utility>

namespace {

auto choose_swapchain_surface_format(
    vk::SurfaceKHR     t_surface,
    vk::PhysicalDevice t_physical_device
) noexcept -> tl::optional<vk::SurfaceFormatKHR>
{
    const auto [result, t_available_surface_formats]{
        t_physical_device.getSurfaceFormatsKHR(t_surface)
    };
    if (result != vk::Result::eSuccess) {
        return tl::nullopt;
    }

    for (const auto& surface_format : t_available_surface_formats) {
        if (surface_format.format == vk::Format::eB8G8R8A8Srgb
            && surface_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return surface_format;
        }
    }
    return t_available_surface_formats.front();
}

auto choose_swapchain_present_mode(
    vk::SurfaceKHR     t_surface,
    vk::PhysicalDevice t_physical_device
) noexcept -> tl::optional<vk::PresentModeKHR>
{
    const auto [result, present_modes]{
        t_physical_device.getSurfacePresentModesKHR(t_surface)
    };
    if (result != vk::Result::eSuccess) {
        return tl::nullopt;
    }
    return std::ranges::find(present_modes, vk::PresentModeKHR::eMailbox)
                != std::cend(present_modes)
             ? vk::PresentModeKHR::eMailbox
             : vk::PresentModeKHR::eFifo;
}

auto create_swapchain(
    vk::SurfaceKHR             t_surface,
    vk::PhysicalDevice         t_physical_device,
    uint32_t                   t_graphics_queue_family,
    uint32_t                   t_present_queue_family,
    vk::Device                 t_device,
    vk::SurfaceCapabilitiesKHR t_surface_capabilities,
    vk::Extent2D               t_extent,
    vk::SurfaceFormatKHR       t_surfaceFormat,
    vk::SwapchainKHR           t_old_swapchain
) noexcept -> vk::SwapchainKHR
{
    const auto present_mode{
        choose_swapchain_present_mode(t_surface, t_physical_device)
    };
    if (!present_mode.has_value()) {
        return nullptr;
    }

    uint32_t image_count = t_surface_capabilities.minImageCount + 1;
    if (t_surface_capabilities.maxImageCount > 0
        && image_count > t_surface_capabilities.maxImageCount)
    {
        image_count = t_surface_capabilities.maxImageCount;
    }

    const std::set buffer{ t_graphics_queue_family, t_present_queue_family };
    const std::vector<uint32_t> queueFamilyIndices = { buffer.cbegin(),
                                                       buffer.cend() };
    const vk::SharingMode       sharingMode = queueFamilyIndices.size() > 1
                                                ? vk::SharingMode::eConcurrent
                                                : vk::SharingMode::eExclusive;

    const vk::SwapchainCreateInfoKHR create_info{
        .surface          = t_surface,
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
        .presentMode         = *present_mode,
        .clipped             = true,
        .oldSwapchain        = t_old_swapchain
    };

    const auto swapchain{ t_device.createSwapchainKHR(create_info) };
    if (swapchain.result != vk::Result::eSuccess) {
        return nullptr;
    }

    return swapchain.value;
}

auto create_image_views(
    vk::Device           t_device,
    vk::SwapchainKHR     t_swapchain,
    vk::SurfaceFormatKHR t_surface_format
) noexcept -> tl::optional<std::vector<engine::vulkan::ImageView>>
{
    const auto images = t_device.getSwapchainImagesKHR(t_swapchain);
    if (images.result != vk::Result::eSuccess) {
        return tl::nullopt;
    }
    std::vector<engine::vulkan::ImageView> image_views;
    image_views.reserve(images.value.size());

    vk::ImageViewCreateInfo image_view_create_info{
        .viewType         = vk::ImageViewType::e2D,
        .format           = t_surface_format.format,
        .subresourceRange = { .aspectMask     = vk::ImageAspectFlagBits::eColor,
                             .baseMipLevel   = 0,
                             .levelCount     = 1,
                             .baseArrayLayer = 0,
                             .layerCount     = 1 }
    };
    for (auto image : images.value) {
        image_view_create_info.image = image;
        auto image_view{ t_device.createImageView(image_view_create_info) };
        if (image_view.result != vk::Result::eSuccess) {
            return tl::nullopt;
        }
        image_views.emplace_back(t_device, image_view.value);
    }

    return image_views;
}

}   // namespace

namespace engine::vulkan {

////////////////////////////////////
///------------------------------///
///  SwapchainHolder   IMPLEMENTATION  ///
///------------------------------///
////////////////////////////////////

auto SwapchainHolder::choose_extent(
    const vk::Extent2D&               t_framebuffer_size,
    const vk::SurfaceCapabilitiesKHR& t_surface_capabilities
) noexcept -> vk::Extent2D
{
    if (t_surface_capabilities.currentExtent.width
        != std::numeric_limits<uint32_t>::max())
    {
        return t_surface_capabilities.currentExtent;
    }
    else {
        vk::Extent2D actual_extent{
            .width  = static_cast<uint32_t>(t_framebuffer_size.width),
            .height = static_cast<uint32_t>(t_framebuffer_size.height)
        };

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

        return actual_extent;
    }
}

auto SwapchainHolder::create(
    vk::SurfaceKHR     t_surface,
    vk::PhysicalDevice t_physical_device,
    uint32_t           t_graphics_queue_family,
    uint32_t           t_present_queue_family,
    vk::Device         t_device,
    vk::Extent2D       t_framebuffer_size,
    vk::SwapchainKHR   t_old_swapchain
) noexcept -> tl::optional<SwapchainHolder>
{
    const auto [result, surface_capabilities]{
        t_physical_device.getSurfaceCapabilitiesKHR(t_surface)
    };
    if (result != vk::Result::eSuccess) {
        return tl::nullopt;
    }

    const auto extent = choose_extent(t_framebuffer_size, surface_capabilities);
    if (extent.width == 0 || extent.height == 0) {
        return tl::nullopt;
    }

    const auto surface_format{
        choose_swapchain_surface_format(t_surface, t_physical_device)
    };
    if (!surface_format.has_value()) {
        return tl::nullopt;
    }

    const auto swapchain{ create_swapchain(
        t_surface,
        t_physical_device,
        t_graphics_queue_family,
        t_present_queue_family,
        t_device,
        surface_capabilities,
        extent,
        *surface_format,
        t_old_swapchain
    ) };
    if (!swapchain) {
        return tl::nullopt;
    }

    auto image_views{
        create_image_views(t_device, swapchain, *surface_format)
    };
    if (!image_views.has_value()) {
        return tl::nullopt;
    }

    return SwapchainHolder{
        t_device,
        extent,
        *surface_format,
        Swapchain{ t_device, swapchain },
        std::move(*image_views)
    };
}

auto SwapchainHolder::operator*() const noexcept -> vk::SwapchainKHR
{
    return m_swapchain.operator*();
}

auto SwapchainHolder::extent() const noexcept -> vk::Extent2D
{
    return m_extent;
}

auto SwapchainHolder::surface_format() const noexcept -> vk::SurfaceFormatKHR
{
    return m_surface_format;
}

auto SwapchainHolder::image_views() const noexcept
    -> const std::vector<ImageView>&
{
    return m_image_views;
}

SwapchainHolder::SwapchainHolder(
    vk::Device               t_device,
    vk::Extent2D             t_extent,
    vk::SurfaceFormatKHR     t_surface_format,
    Swapchain&&              t_swapchain,
    std::vector<ImageView>&& t_image_views
) noexcept
    : m_device{ t_device },
      m_extent{ t_extent },
      m_surface_format{ t_surface_format },
      m_swapchain{ std::move(t_swapchain) },
      m_image_views{ std::move(t_image_views) }
{}

}   // namespace engine::vulkan