#include "window.hpp"

namespace engine::window {

WindowPlugin::WindowPlugin(
    sf::VideoMode                 t_video_mode,
    std::string                   t_title,
    engine::window::Window::Style t_style
) noexcept
    : m_video_mode{ t_video_mode },
      m_title{ std::move(t_title) },
      m_style{ t_style }
{}

auto WindowPlugin::set_context(App::Context& t_context) const noexcept -> void
{
    auto window{ Window::create(m_video_mode, m_title, m_style) };

    if (window.has_value()) {
        t_context.emplace<Window>(std::move(*window));
    }
}


}   // namespace engine::window
