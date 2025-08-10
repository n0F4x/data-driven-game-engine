module;

#include <utility>

#include <SFML/Window.hpp>

export module snake.window.update;

import ddge.modules.time.FixedTimer;

import ddge.modules.scheduler.accessors.events.Recorder;
import ddge.modules.scheduler.accessors.resources;

import snake.window.DisplayTimer;
import snake.window.Window;

using namespace ddge::scheduler::accessors;

namespace window {

export auto update(
    resources::Resource<Window>                                    window,
    const ddge::scheduler::accessors::events::Recorder<sf::Event>& event_recorder,
    resources::Resource<DisplayTimer>                              display_timer
) -> void;

}   // namespace window

module :private;

auto window::update(
    const resources::Resource<Window>                              window,
    const ddge::scheduler::accessors::events::Recorder<sf::Event>& event_recorder,
    const resources::Resource<DisplayTimer>                        display_timer
) -> void
{
    window->handleEvents([&event_recorder]<typename Event>(Event&& event) -> void {
        event_recorder.record(std::forward<Event>(event));
    });

    display_timer->update();
}
