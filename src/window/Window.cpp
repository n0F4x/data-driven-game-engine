#include "Window.hpp"

namespace window {

Window::Window(const Builder& t_builder)
    : sf::WindowBase{ t_builder.video_mode(),
                      t_builder.title(),
                      t_builder.style() } {}

auto Window::create() noexcept -> Builder {
    return Builder{};
}

auto Window::Builder::build() const -> Window {
    return Window{ *this };
}

auto Window::Builder::set_video_mode(const sf::VideoMode& t_video_mode) noexcept
    -> Window::Builder& {
    m_video_mode = t_video_mode;
    return *this;
}

auto Window::Builder::set_title(const sf::String& t_title) noexcept
    -> Window::Builder& {
    m_title = t_title;
    return *this;
}

auto Window::Builder::set_style(sf::Uint32 t_style) noexcept
    -> Window::Builder& {
    m_style = t_style;
    return *this;
}

auto Window::Builder::style() const noexcept -> sf::Uint32 {
    return m_style;
}

auto Window::Builder::title() const noexcept -> const sf::String& {
    return m_title;
}

auto Window::Builder::video_mode() const noexcept -> const sf::VideoMode& {
    return m_video_mode;
}

}   // namespace window
