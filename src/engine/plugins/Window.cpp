#include "Window.hpp"

#include <spdlog/spdlog.h>

namespace engine::plugins {

auto Window::operator()(
    App::Store&           t_store,
    const sf::VideoMode&  t_video_mode,
    std::string_view      t_title,
    window::Style        t_style
) noexcept -> void
{
    window::Window::create(t_video_mode, t_title.data(), t_style)
        .transform([&t_store](window::Window&& t_window) {
            t_store.emplace<window::Window>(std::move(t_window));
        })
        .transform([](auto&&) { SPDLOG_TRACE("Added Window plugin"); });
}

}   // namespace engine::plugins
