#include <spdlog/spdlog.h>

#include "app/core/Builder.hpp"
#include "engine/window/Window.hpp"

#include "Window.hpp"

using namespace engine::window;

namespace app::plugins {

auto Window::default_configure() -> void
{
    glfwSetErrorCallback([](int, const char* description) { SPDLOG_ERROR(description); });

    if (auto error_code{ glfwInit() }; error_code != GLFW_TRUE) {
        throw std::runtime_error{
            std::format("glfwInit failed with error code {}", std::to_string(error_code))
        };
    }
    std::atexit(glfwTerminate);

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

auto Window::operator()(
    App::Builder&      t_builder,
    uint16_t           t_width,
    uint16_t           t_height,
    const std::string& t_title
) -> void
{
    t_builder.store().emplace<engine::window::Window>(t_width, t_height, t_title);
    SPDLOG_TRACE("Added Window plugin");
}

}   // namespace engine::window
