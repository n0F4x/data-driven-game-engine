module;

#include <chrono>

#include <GLFW/glfw3.h>

module core.window.events;

namespace core::window {

auto poll_events() -> void
{
    glfwPollEvents();
}

auto wait_for_events() -> void
{
    glfwWaitEvents();
}

auto wait_for_events(const std::chrono::duration<float> timeout) -> void
{
    glfwWaitEventsTimeout(timeout.count());
}

}   // namespace core::window
