#include "Window.hpp"

#include <format>
#include <stdexcept>

#include <spdlog/spdlog.h>

static auto init_glfw() -> void
{
    if (const int success{ glfwInit() }; success != GLFW_TRUE) {
        const char* description{};
        const int   error_code{ glfwGetError(&description) };
        // TODO: [[assume(description != null)]]
        throw std::runtime_error{ std::format(
            "glfwInit failed with error code {} - '{}'", error_code, description
        ) };
    }

    if (const int result{ std::atexit(glfwTerminate) }; result != 0) {
        SPDLOG_ERROR("std::atexit failed with error code {}", result);
    }
}

namespace core::window {

auto Window::vulkan_instance_extensions()
    -> const std::vector<gsl_lite::not_null<gsl_lite::czstring>>&
{
    static const std::vector s_extension_names{
        [] -> std::vector<gsl_lite::not_null<gsl_lite::czstring>> {
            init_glfw();

            if (glfwVulkanSupported() != GLFW_TRUE) {
                throw std::runtime_error{ "glfwVulkanSupported returned false" };
            }

            uint32_t count{};
            const char** glfw_extension_names{ glfwGetRequiredInstanceExtensions(&count) };
            if (glfw_extension_names == nullptr) {
                return {};
            }

            return std::vector<gsl_lite::not_null<gsl_lite::czstring>>{
                glfw_extension_names, std::next(glfw_extension_names, count)
            };
        }()
    };

    return s_extension_names;
}

[[nodiscard]]
static auto create_window(
    const uint16_t     t_width,
    const uint16_t     t_height,
    const std::string& t_title
) -> GLFWwindow*
{
    init_glfw();

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
