module;

#include <string_view>
#include <utility>

#include <SFML/Graphics.hpp>

export module demo.window.Window;

import demo.window.events;
import demo.window.Settings;

import extensions.scheduler.accessors.events.Recorder;

namespace window {

export using EventRecorder =
    extensions::scheduler::accessors::events::Recorder<events::CloseRequested>;

export class Window {
public:
    constexpr explicit Window(Settings settings) : m_settings{ std::move(settings) } {}

    auto set_title(std::string_view title) -> void;

    auto open() -> void;
    auto close() -> void;

    auto clear(sf::Color clear_color = sf::Color::Black) -> void;
    auto draw(const sf::Drawable& drawable) -> void;
    auto display() -> void;

    auto record_events(EventRecorder event_recorder);

    [[nodiscard]]
    auto is_open() const -> bool;

    [[nodiscard]]
    auto width() const -> uint16_t;
    [[nodiscard]]
    auto height() const -> uint16_t;

private:
    Settings         m_settings;
    sf::RenderWindow m_window;
};

}   // namespace window

module :private;

auto window::Window::set_title(const std::string_view title) -> void
{
    m_settings.title = title;
    m_window.setTitle(std::string{ m_settings.title });
}

auto window::Window::open() -> void
{
    m_window.create(
        sf::VideoMode(sf::Vector2u{ m_settings.width, m_settings.height }),
        std::string{ m_settings.title }
    );
}

auto window::Window::close() -> void
{
    m_window.close();
}

auto window::Window::clear(sf::Color clear_color) -> void
{
    m_window.clear(clear_color);
}

auto window::Window::draw(const sf::Drawable& drawable) -> void
{
    m_window.draw(drawable);
}

auto window::Window::display() -> void
{
    m_window.display();
}

auto window::Window::record_events(const ::window::EventRecorder event_recorder)
{
    m_window.handleEvents([&event_recorder](sf::Event::Closed) {
        event_recorder.record<events::CloseRequested>();
    });
}

auto window::Window::is_open() const -> bool
{
    return m_window.isOpen();
}

auto window::Window::width() const -> uint16_t
{
    return m_settings.width;
}

auto window::Window::height() const -> uint16_t
{
    return m_settings.height;
}
