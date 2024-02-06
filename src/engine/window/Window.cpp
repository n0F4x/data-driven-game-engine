#include "Window.hpp"

#include <cstdlib>
#include <format>
#include <stdexcept>

#include <spdlog/spdlog.h>

namespace engine::window {

/////////////////////////////////
///---------------------------///
///  Window   IMPLEMENTATION  ///
///---------------------------///
/////////////////////////////////

[[nodiscard]] static auto
    create_window(uint16_t t_width, uint16_t t_height, const std::string& title)
        -> GLFWwindow*
{
    auto window{
        glfwCreateWindow(t_width, t_height, title.c_str(), nullptr, nullptr)
    };
    if (window == nullptr) {
        throw std::runtime_error{ std::format(
            "glfwCreateWindowSurface failed with error code {}",
            std::to_string(glfwGetError(nullptr))
        ) };
    }
    return window;
}

Window::Window(uint16_t t_width, uint16_t t_height, const std::string& title)
    : m_impl{ create_window(t_width, t_height, title), glfwDestroyWindow }
{}

auto Window::get() const noexcept -> GLFWwindow*
{
    return m_impl.get();
}

}   // namespace engine::window
