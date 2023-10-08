#include "Window.hpp"

namespace engine::plugins {

auto Window::setup(
    App::Context&         t_context,
    const sf::VideoMode&  t_video_mode,
    std::string_view      t_title,
    window::Window::Style t_style
) noexcept -> void
{
    auto window{
        window::Window::create(t_video_mode, t_title.data(), t_style)
    };

    if (window.has_value()) {
        t_context.emplace<window::Window>(std::move(*window));
    }
}

}   // namespace engine::plugins
