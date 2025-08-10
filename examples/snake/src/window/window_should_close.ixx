module;

#include <SFML/Window/Event.hpp>

export module snake.window.window_should_close;

import ddge.modules.scheduler.accessors.events.Reader;

using namespace ddge::scheduler::accessors;

namespace window {

export auto window_should_close(const ::events::Reader<sf::Event>& closed_event_reader)
    -> bool;

}   // namespace window

module :private;

auto window::window_should_close(const events::Reader<sf::Event>& closed_event_reader)
    -> bool
{
    for (const sf::Event& event : closed_event_reader.read()) {
        if (event.is<sf::Event::Closed>()) {
            return true;
        }
    }
    return false;
}
