module;

#include <cassert>
#include <cstdint>
#include <utility>

#include <GLFW/glfw3.h>

export module ddge.modules.wsi.VulkanWindow;

import vulkan_hpp;

import ddge.modules.vulkan.Device;
import ddge.modules.vulkan.result.check_result;
import ddge.modules.vulkan.Swapchain;
import ddge.modules.wsi.Context;
import ddge.modules.wsi.Window;

namespace ddge::wsi {

export class VulkanWindow : public Window {
public:
    VulkanWindow(
        Window&&                  window,
        const vk::raii::Instance& vulkan_instance,
        const vulkan::Device&     vulkan_device,
        uint32_t                  number_of_frames
    );

    template <vulkan::present_mode_picker_c PickPresentMode_T>
    VulkanWindow(
        Window&&                  window,
        const vk::raii::Instance& vulkan_instance,
        const vulkan::Device&     vulkan_device,
        uint32_t                  number_of_frames,
        PickPresentMode_T&&       pick_present_mode
    );

private:
    vk::raii::SurfaceKHR m_surface;
    vulkan::Swapchain    m_swapchain;
};

}   // namespace ddge::wsi

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

VulkanWindow::VulkanWindow(
    Window&&                  window,
    const vk::raii::Instance& vulkan_instance,
    const vulkan::Device&     vulkan_device,
    const uint32_t            number_of_frames
)
    : VulkanWindow{
          std::move(window),
          vulkan_instance,
          vulkan_device,
          number_of_frames,
          vulkan::Swapchain::pick_present_mode,
      }
{}

template <vulkan::present_mode_picker_c PickPresentMode_T>
VulkanWindow::VulkanWindow(
    Window&&                  window,
    const vk::raii::Instance& vulkan_instance,
    const vulkan::Device&     vulkan_device,
    const uint32_t            number_of_frames,
    PickPresentMode_T&&       pick_present_mode
)
    : Window{
          std::move(window)
},
      m_surface{ create_surface(context(), handle(), vulkan_instance) },
      m_swapchain{
          m_surface,
          vulkan_device,
          vk::Extent2D{
              .width  = static_cast<uint32_t>(resolution().width),
              .height = static_cast<uint32_t>(resolution().height),
          },
          number_of_frames,
          pick_present_mode,
      }
{}

}   // namespace ddge::wsi
