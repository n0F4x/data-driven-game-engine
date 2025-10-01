module;

#include <utility>

#include <SFML/Window.hpp>

module snake.window.tasks.update;

import ddge.modules.time.FixedTimer;

import ddge.modules.execution.accessors.events.Recorder;
import ddge.modules.execution.accessors.resources;
import ddge.modules.execution.providers.EventProvider;
import ddge.modules.execution.providers.ResourceProvider;
import ddge.modules.execution.v2.primitives.force_on_main;

import snake.window.DisplayTimer;
import snake.window.Window;

using namespace ddge::exec::accessors;

auto window::update() -> ddge::exec::v2::TaskBuilder<void>
{
    return ddge::exec::v2::force_on_main(
        +[](   //
             const resources::Resource<Window>                         window,
             const ddge::exec::accessors::events::Recorder<sf::Event>& event_recorder,
             const resources::Resource<DisplayTimer>                   display_timer
         ) -> void   //
        {
            window->handleEvents([&event_recorder]<typename Event>(Event&& event) -> void {
                event_recorder.record(std::forward<Event>(event));
            });

            display_timer->update();
        }   //
    );
}
