#include "Plugin.hpp"

#include <spdlog/spdlog.h>

#include "Window.hpp"

namespace engine::window {

auto Plugin::operator()(
    Store&               t_store,
    const sf::VideoMode& t_video_mode,
    std::string_view     t_title,
    Style                t_style
) noexcept -> void
{
    Window::create(t_video_mode, t_title.data(), t_style)
        .transform([&t_store](Window&& t_window) {
            t_store.emplace<Window>(std::move(t_window));
        })
        .transform([](auto&&) { SPDLOG_TRACE("Added Window plugin"); });
}

}   // namespace engine::window
