#include "events.hpp"

#include <GLFW/glfw3.h>

namespace core::window {

auto poll_events() -> void
{
    glfwPollEvents();
}

auto wait_for_events() -> void
{
    glfwWaitEvents();
}

auto wait_for_events(std::chrono::duration<float> timeout) -> void
{
    glfwWaitEventsTimeout(timeout.count());
}

}   // namespace core::window
