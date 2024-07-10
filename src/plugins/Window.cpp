#include "Window.hpp"

#include <spdlog/spdlog.h>

#include "core/window/Window.hpp"

namespace plugins {

auto Window::operator()(Store& store, const core::Size2i& size, gsl_lite::czstring title)
    const -> void
{
    store.emplace<core::window::Window>(size, title);
    SPDLOG_TRACE("Added Window plugin");
}

}   // namespace plugins
