#include "Window.hpp"

namespace engine::plugins {

Window::Window(
    sf::VideoMode         t_video_mode,
    std::string_view      t_title,
    window::Window::Style t_style
) noexcept
    : m_video_mode{ t_video_mode },
      m_title{ t_title },
      m_style{ t_style }
{}

auto Window::set_context(App::Context& t_context) const noexcept -> void
{
    auto window{
        window::Window::create(m_video_mode, m_title.data(), m_style)
    };

    if (window.has_value()) {
        t_context.emplace<window::Window>(std::move(*window));
    }
}

}   // namespace engine::plugins
