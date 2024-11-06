#include "Window.hpp"

#include <spdlog/spdlog.h>

#include "app/App.hpp"
#include "core/window/Window.hpp"

namespace plugins::window {

auto Window::operator()() const -> core::window::Window
{
    return core::window::Window{size, title};
}

}   // namespace plugins::window
