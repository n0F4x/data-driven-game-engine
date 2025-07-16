module;

#include <SFML/Window/Event.hpp>

export module snake.window.window_should_close;

import extensions.scheduler.accessors.events.Reader;

using namespace extensions::scheduler::accessors;

namespace window {

export inline constexpr auto window_should_close =   //
    [](const ::events::Reader<sf::Event>& closed_event_reader) {
        for (const sf::Event& event : closed_event_reader.read()) {
            if (event.is<sf::Event::Closed>()) {
                return true;
            }
        }
        return false;
    };

}   // namespace window
