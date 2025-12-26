module;

#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstdint>
#include <limits>
#include <print>
#include <type_traits>
#include <utility>
#include <vector>

#include <GLFW/glfw3.h>

#include "utility/contract_macros.hpp"

export module ddge.modules.wsi.VulkanWindow;

import vulkan_hpp;

import ddge.modules.vulkan.Device;
import ddge.modules.vulkan.result.check_result;
import ddge.modules.wsi.Context;
import ddge.modules.wsi.Size;
import ddge.modules.wsi.Window;
import ddge.utility.contracts;

namespace ddge::wsi {

template <typename T>
concept present_mode_picker_c = requires {
    std::same_as<
        std::invoke_result_t<T&&, const std::vector<vk::PresentModeKHR>&>,
        vk::PresentModeKHR>;
};

export class VulkanWindow : public Window {
public:
    VulkanWindow(
        Window&&                  window,
        const vk::raii::Instance& vulkan_instance,
        const vulkan::Device&     vulkan_device
    );

    template <present_mode_picker_c PickPresentMode_T>
    VulkanWindow(
        Window&&                  window,
        const vk::raii::Instance& vulkan_instance,
        const vulkan::Device&     vulkan_device,
        PickPresentMode_T&&       pick_present_mode
    );

private:
    vk::raii::SurfaceKHR   m_surface;
    vk::raii::SwapchainKHR m_swapchain;
    std::vector<vk::Image> m_swapchain_images;
};

}   // namespace ddge::wsi

module :private;

namespace ddge::wsi {

[[nodiscard]]
auto create_surface(const Context&, GLFWwindow& window, const vk::raii::Instance& instance)
    -> vk::raii::SurfaceKHR
{
    VkSurfaceKHR surface{};
    vulkan::check_result(glfwCreateWindowSurface(*instance, &window, nullptr, &surface));

    [[maybe_unused]]
    const int error_code = glfwGetError(nullptr);
    assert(
        (error_code != GLFW_API_UNAVAILABLE && error_code != GLFW_INVALID_VALUE
         && error_code != GLFW_PLATFORM_ERROR)
        && "A precondition should have already been violated"
    );
    assert(error_code == GLFW_NO_ERROR);

    return vk::raii::SurfaceKHR{ instance, surface };
}

[[nodiscard]]
auto pick_surface_format(const std::vector<vk::SurfaceFormatKHR>& surface_formats)
    -> vk::SurfaceFormatKHR
{
    for (const vk::SurfaceFormatKHR& surface_format : surface_formats) {
        if (surface_format.format == vk::Format::eB8G8R8A8Srgb
            && surface_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            return surface_format;
        }
    }
    return surface_formats.front();
}

[[nodiscard]]
auto pick_present_mode(const std::vector<vk::PresentModeKHR>& present_modes)
    -> vk::PresentModeKHR
{
    return std::ranges::contains(present_modes, vk::PresentModeKHR::eMailbox)
             ? vk::PresentModeKHR::eMailbox
         : std::ranges::contains(present_modes, vk::PresentModeKHR::eFifoRelaxed)
             ? vk::PresentModeKHR::eFifoRelaxed
             : vk::PresentModeKHR::eFifo;
}

[[nodiscard]]
auto pick_swap_extent(
    const Window&                     window,
    const vk::SurfaceCapabilitiesKHR& surface_capabilities
) -> vk::Extent2D
{
    if (surface_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return surface_capabilities.currentExtent;
    }

    const Size2i framebuffer_size{ window.resolution() };

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

template <present_mode_picker_c PickPresentMode_T>
[[nodiscard]]
auto create_swapchain(
    const Window&               window,
    const vk::raii::SurfaceKHR& surface,
    const vulkan::Device&       device,
    PickPresentMode_T&&         pick_present_mode
) -> vk::raii::SwapchainKHR
{
    PRECOND(
        device.logical_device.getDispatcher()->vkCreateSwapchainKHR != nullptr,
        "VK_KHR_SWAPCHAIN_EXTENSION_NAME was not enabled"
    );

    const vk::SurfaceCapabilitiesKHR surface_capabilities{
        device.physical_device.getSurfaceCapabilitiesKHR(surface)
    };
    const vk::SurfaceFormatKHR surface_format{
        pick_surface_format(device.physical_device.getSurfaceFormatsKHR(surface))
    };
    const vk::PresentModeKHR present_mode{
        std::invoke(
            std::forward<PickPresentMode_T>(pick_present_mode),
            device.physical_device.getSurfacePresentModesKHR(surface)
        )   //
    };
    const uint32_t image_count{
        surface_capabilities.maxImageCount == 0
            ? surface_capabilities.minImageCount + 1
            : std::min(
                  surface_capabilities.minImageCount + 1,
                  surface_capabilities.maxImageCount
              )   //
    };

    const vk::SwapchainCreateInfoKHR create_info{
        .surface          = surface,
        .minImageCount    = image_count,
        .imageFormat      = surface_format.format,
        .imageColorSpace  = surface_format.colorSpace,
        .imageExtent      = pick_swap_extent(window, surface_capabilities),
        .imageArrayLayers = 1,
        .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform     = surface_capabilities.currentTransform,
        .compositeAlpha   = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode      = present_mode,
        .clipped          = vk::True,
    };

    return vulkan::check_result(device.logical_device.createSwapchainKHR(create_info));
}

VulkanWindow::VulkanWindow(
    Window&&                  window,
    const vk::raii::Instance& vulkan_instance,
    const vulkan::Device&     vulkan_device
)
    : VulkanWindow{ std::move(window), vulkan_instance, vulkan_device, pick_present_mode }
{}

template <present_mode_picker_c PickPresentMode_T>
VulkanWindow::VulkanWindow(
    Window&&                  window,
    const vk::raii::Instance& vulkan_instance,
    const vulkan::Device&     vulkan_device,
    PickPresentMode_T&&       pick_present_mode
)
    : Window{ std::move(window) },
      m_surface{ create_surface(context(), handle(), vulkan_instance) },
      m_swapchain{ create_swapchain(
          *this,
          m_surface,
          vulkan_device,
          std::forward<PickPresentMode_T>(pick_present_mode)
      ) },
      m_swapchain_images{ m_swapchain.getImages() }
{}

}   // namespace ddge::wsi
