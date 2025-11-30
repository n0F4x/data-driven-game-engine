module;

#include <SFML/Window/Event.hpp>

module snake.window.tasks.window_should_close;

import ddge.modules.scheduler.accessors.events;
import ddge.modules.scheduler.primitives.as_task;

using namespace ddge::scheduler::accessors;

auto window::tasks::window_should_close()
    -> ddge::scheduler::TaskBlueprint<bool, ddge::scheduler::Cardinality::eSingle>
{
    return ddge::scheduler::as_task(
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
