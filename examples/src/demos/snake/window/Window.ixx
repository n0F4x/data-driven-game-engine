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

    auto clear() -> void;
    auto display() -> void;

    auto record_events(EventRecorder event_recorder);

    [[nodiscard]]
    auto is_open() const -> bool;

private:
    Settings         m_settings;
    sf::RenderWindow m_window;
};

}   // namespace window

auto window::Window::set_title(const std::string_view title) -> void
{
    m_settings.title = title;
    m_window.setTitle(m_settings.title);
}

auto window::Window::open() -> void
{
    m_window.create(
        sf::VideoMode(sf::Vector2u{ m_settings.width, m_settings.height }),
        m_settings.title
    );
}

auto window::Window::close() -> void
{
    m_window.close();
}

auto window::Window::clear() -> void
{
    m_window.clear();
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
