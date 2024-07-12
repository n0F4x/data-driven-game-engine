#include "Window.hpp"

#include <spdlog/spdlog.h>

#include "core/window/Window.hpp"

namespace plugins {

auto Window::operator()(App& app) const -> void
{
    app.plugins().emplace<core::window::Window>(size, title);
    SPDLOG_TRACE("Added Window plugin");
}

}   // namespace plugins
