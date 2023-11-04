#include "Window.hpp"

#include <spdlog/spdlog.h>

namespace engine::plugins {

auto Window::operator()(
    App::Store&           t_store,
    const sf::VideoMode&  t_video_mode,
    std::string_view      t_title,
    window::Window::Style t_style
) noexcept -> void
{
    auto window{
        window::Window::create(t_video_mode, t_title.data(), t_style)
    };
    if (!window.has_value()) {
        return;
    }

    t_store.emplace<window::Window>(std::move(*window));

    SPDLOG_TRACE("Added Window plugin");
}

}   // namespace engine::plugins
