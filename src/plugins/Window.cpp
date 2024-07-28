#include "Window.hpp"

#include <spdlog/spdlog.h>

#include "app/App.hpp"
#include "core/window/Window.hpp"

namespace plugins::window {

auto Window::operator()(App& app) const -> void
{
    app.resources.emplace<core::window::Window>(size, title);
}

}   // namespace plugins::window
