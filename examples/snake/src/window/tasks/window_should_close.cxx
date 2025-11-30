module;

#include <SFML/Window/Event.hpp>

module snake.window.tasks.window_should_close;

import ddge.modules.exec.accessors.events;
import ddge.modules.exec.primitives.as_task;

using namespace ddge::exec::accessors;

auto window::tasks::window_should_close()
    -> ddge::exec::TaskBlueprint<bool, ddge::exec::Cardinality::eSingle>
{
    return ddge::exec::as_task(
        +[](const events::Reader<sf::Event>& closed_event_reader) -> bool   //
        {
            for (const sf::Event& event : closed_event_reader.read()) {
                if (event.is<sf::Event::Closed>()) {
                    return true;
                }
            }
            return false;
        }   //
    );
}
