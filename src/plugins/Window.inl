#include <spdlog/spdlog.h>

#include "core/window/Window.hpp"

namespace plugins {

auto Window::operator()(
    Store&              store,
    const uint16_t      width,
    const uint16_t      height,
    const std::string&  title,
    std::invocable auto configure
) const -> void
{
    std::invoke(configure);
    store.emplace<core::window::Window>(width, height, title);
    SPDLOG_TRACE("Added Window plugin");
}

}   // namespace plugins
