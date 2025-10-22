module;

// TODO: remove once Clang can mangle
#include <function2/function2.hpp>

#include <SFML/Window/Event.hpp>

module snake.window.tasks.window_should_close;

import ddge.modules.execution.accessors.events.Reader;
import ddge.modules.execution.providers.EventProvider;
import ddge.modules.execution.v2.primitives.as_task;

using namespace ddge::exec::accessors;

auto window::tasks::window_should_close()
    -> ddge::exec::v2::TaskBlueprint<bool, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::as_task(
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
