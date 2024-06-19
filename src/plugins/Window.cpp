#include "core/window/Window.hpp"

#include <spdlog/spdlog.h>

#include "app/Builder.hpp"
#include "core/config/vulkan.hpp"

#include "Window.hpp"

using namespace core::window;

namespace plugins {

auto Window::default_configure() -> void
{
    glfwSetErrorCallback([](int, const char* description) { SPDLOG_ERROR(description); });

    glfwInitVulkanLoader(core::config::vulkan::instance_proc_address());

    if (const auto error_code{ glfwInit() }; error_code != GLFW_TRUE) {
        throw std::runtime_error{
            std::format("glfwInit failed with error code {}", error_code)
        };
    }
    if (auto result{ std::atexit(glfwTerminate) }; result != 0) {
        throw std::runtime_error{
            std::format("std::atexit(glfwTerminate) failed with error code {}", result)
        };
    }

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

auto Window::operator()(
    app::App::Builder& t_builder,
    uint16_t           t_width,
    uint16_t           t_height,
    const std::string& t_title
) const -> void
{
    t_builder.store().emplace<core::window::Window>(t_width, t_height, t_title);
    SPDLOG_TRACE("Added Window plugin");
}

}   // namespace plugins
