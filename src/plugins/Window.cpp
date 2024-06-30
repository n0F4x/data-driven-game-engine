#include "Window.hpp"

#include <spdlog/spdlog.h>

#include "core/config/vulkan.hpp"

namespace plugins {

static auto default_configure() -> void
{
    glfwSetErrorCallback([](int, const char* description) { SPDLOG_ERROR(description); });

    glfwInitVulkanLoader(core::config::vulkan::dispatcher().vkGetInstanceProcAddr);

    if (const auto error_code{ glfwInit() }; error_code != GLFW_TRUE) {
        throw std::runtime_error{
            std::format("glfwInit failed with error code {}", error_code)
        };
    }
    if (const auto result{ std::atexit(glfwTerminate) }; result != 0) {
        throw std::runtime_error{
            std::format("std::atexit(glfwTerminate) failed with error code {}", result)
        };
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

auto Window::operator()(
    App::Builder&      t_builder,
    uint16_t           t_width,
    uint16_t           t_height,
    const std::string& t_title
) const -> void
{
    operator()(t_builder, t_width, t_height, t_title, default_configure);
}

}   // namespace plugins
