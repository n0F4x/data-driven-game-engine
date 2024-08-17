#include "Window.hpp"

#include <format>
#include <stdexcept>

#include <spdlog/spdlog.h>

#include "core/config/vulkan.hpp"

static auto init_glfw() -> void
{
    glfwSetErrorCallback([](int, const char* description) { SPDLOG_ERROR(description); });

    glfwInitVulkanLoader(core::config::vulkan::dispatcher().vkGetInstanceProcAddr);

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
static auto
    create_window(const Size2i& size, const gsl_lite::czstring title) -> GLFWwindow*
{
    init_glfw();

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* const window{
        glfwCreateWindow(size.width, size.height, title, nullptr, nullptr)
    };
    if (window == nullptr) {
        throw std::runtime_error{ std::format(
            "glfwCreateWindowSurface failed with error code {}",
            std::to_string(glfwGetError(nullptr))
        ) };
    }
    return window;
}

Window::Window(const Size2i& size, const gsl_lite::czstring title)
    : m_impl{
          std::unique_ptr<GLFWwindow, decltype(&glfwDestroyWindow)>{
                                                                    create_window(size, title),
                                                                    glfwDestroyWindow }
}
{
    glfwSetWindowUserPointer(m_impl.get(), this);
}

Window::Window(Window&& other) noexcept
    : m_impl(std::move(other.m_impl)),
      m_framebuffer_resized{ std::move(other.m_framebuffer_resized) }
{
    glfwSetWindowUserPointer(m_impl.get(), this);
}

auto Window::operator=(Window&& other) noexcept -> Window&
{
    if (this != &other) {
        m_impl                = std::move(other.m_impl);
        m_framebuffer_resized = std::move(other.m_framebuffer_resized);

        glfwSetWindowUserPointer(m_impl.get(), this);
    }

    return *this;
}

auto Window::get() const noexcept -> GLFWwindow*
{
    return m_impl.get();
}

auto Window::create_vulkan_surface(
    VkInstance                   instance,
    const VkAllocationCallbacks* allocation_callbacks
) const -> std::expected<VkSurfaceKHR, VkResult>
{
    VkSurfaceKHR surface{};

    if (const VkResult error_code{ glfwCreateWindowSurface(
            instance, m_impl.get(), allocation_callbacks, &surface
        ) };
        error_code != VK_SUCCESS)
    {
        return std::unexpected{ error_code };
    }

    return surface;
}

auto Window::size() const -> Size2i
{
    Size2i result{};
    glfwGetWindowSize(m_impl.get(), &result.width, &result.height);
    return result;
}

auto Window::framebuffer_size() const -> Size2i
{
    Size2i result{};
    glfwGetFramebufferSize(m_impl.get(), &result.width, &result.height);
    return result;
}

auto Window::set_cursor_position(const glm::dvec2& position) const -> void
{
    glfwSetCursorPos(m_impl.get(), position.x, position.y);
}

auto Window::set_cursor_mode(CursorMode cursor_mode) const -> void
{
    switch (cursor_mode) {
        case CursorMode::eNormal:
            glfwSetInputMode(m_impl.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        case CursorMode::eDisabled:
            glfwSetInputMode(m_impl.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetInputMode(m_impl.get(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            break;
    }
}

auto Window::should_close() const -> bool
{
    return glfwWindowShouldClose(m_impl.get()) == GLFW_TRUE;
}

auto Window::key_pressed(Key key) const -> bool
{
    return glfwGetKey(m_impl.get(), static_cast<int>(key)) == GLFW_TRUE;
}

auto Window::cursor_position() const -> glm::dvec2
{
    glm::dvec2 result{};
    glfwGetCursorPos(m_impl.get(), &result.x, &result.y);
    return result;
}

}   // namespace core::window
