#include "Window.hpp"

#include <format>
#include <stdexcept>

namespace core::window {

auto Window::vulkan_instance_extensions() -> std::span<const char* const>
{
    static const std::vector s_extension_names{ []() -> std::vector<const char*> {
        if (glfwInit() != GLFW_TRUE) {
            return {};
        }

        uint32_t             count{};
        const char**         glfw_extension_names{ glfwGetRequiredInstanceExtensions(&count) };
        if (glfw_extension_names == nullptr) {
            return {};
        }

        return std::vector<const char*>{ glfw_extension_names,
                                         std::next(glfw_extension_names, count) };
    }() };

    return s_extension_names;
}

[[nodiscard]]
static auto create_window(
    const uint16_t     t_width,
    const uint16_t     t_height,
    const std::string& t_title
) -> GLFWwindow*
{
    auto* const window{
        glfwCreateWindow(t_width, t_height, t_title.c_str(), nullptr, nullptr)
    };
    if (window == nullptr) {
        throw std::runtime_error{ std::format(
            "glfwCreateWindowSurface failed with error code {}",
            std::to_string(glfwGetError(nullptr))
        ) };
    }
    return window;
}

Window::Window(const uint16_t t_width, const uint16_t t_height, const std::string& t_title)
    : m_impl{ create_window(t_width, t_height, t_title), glfwDestroyWindow }
{}

auto Window::get() const noexcept -> GLFWwindow*
{
    return m_impl.get();
}

auto Window::create_vulkan_surface(
    VkInstance                   t_instance,
    const VkAllocationCallbacks* t_allocation_callbacks
) const -> std::expected<VkSurfaceKHR, VkResult>
{
    VkSurfaceKHR surface{};

    if (const VkResult error_code{ glfwCreateWindowSurface(
            t_instance, m_impl.get(), t_allocation_callbacks, &surface
        ) };
        error_code != VK_SUCCESS)
    {
        return std::unexpected{ error_code };
    }

    return surface;
}

}   // namespace core::window
