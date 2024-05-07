#include "Window.hpp"

#include <format>
#include <stdexcept>

namespace core::window {

/////////////////////////////////
///---------------------------///
///  Window   IMPLEMENTATION  ///
///---------------------------///
/////////////////////////////////

[[nodiscard]]
static auto create_window(
    const uint16_t     t_width,
    const uint16_t     t_height,
    const std::string& t_title
) -> GLFWwindow*
{
    const auto window{
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

}   // namespace core::window
