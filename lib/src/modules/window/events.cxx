module;

#include <chrono>

#include <GLFW/glfw3.h>

module ddge.modules.window.events;

namespace ddge::window {

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

}   // namespace ddge::window
