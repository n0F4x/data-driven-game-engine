#include "Plugin.hpp"

#include <spdlog/spdlog.h>

#include "Window.hpp"

namespace engine::window {

auto Plugin::default_configure() -> void
{
    glfwSetErrorCallback([](int, const char* description) {
        SPDLOG_ERROR(description);
    });

    if (auto error_code{ glfwInit() }; error_code != GLFW_TRUE) {
        throw std::runtime_error{ std::format(
            "glfwInit failed with error code {}", std::to_string(error_code)
        ) };
    }
    std::atexit(glfwTerminate);

    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
}

auto Plugin::operator()(
    Store&             t_store,
    uint16_t           t_width,
    uint16_t           t_height,
    const std::string& t_title
) -> void
{
    t_store.emplace<Window>(t_width, t_height, t_title);
    SPDLOG_TRACE("Added Window plugin");
}

}   // namespace engine::window
