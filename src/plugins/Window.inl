#include <spdlog/spdlog.h>

#include "app/Builder.hpp"
#include "core/window/Window.hpp"

namespace plugins {

auto Window::operator()(
    App::Builder&       t_builder,
    const uint16_t      t_width,
    const uint16_t      t_height,
    const std::string&  t_title,
    std::invocable auto t_configure
) const -> void
{
    std::invoke(t_configure);
    t_builder.store().emplace<core::window::Window>(t_width, t_height, t_title);
    SPDLOG_TRACE("Added Window plugin");
}

}   // namespace plugins
