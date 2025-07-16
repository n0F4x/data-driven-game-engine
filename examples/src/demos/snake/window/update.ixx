module;

#include <utility>

#include <SFML/Window.hpp>

export module snake.window.update;

import core.scheduler;
import core.time.FixedTimer;

import extensions.scheduler.accessors.events.Recorder;
import extensions.scheduler.accessors.resources;

import snake.window.DisplayTimer;
import snake.window.Window;

using namespace extensions::scheduler::accessors;

namespace window {

export inline constexpr auto update =
    [](const resources::Resource<Window> window,
       const extensions::scheduler::accessors::events::Recorder<sf::Event>& event_recorder,
       const resources::Resource<DisplayTimer> display_timer)   //
{
    window->handleEvents([&event_recorder]<typename Event>(Event&& event) {
        event_recorder.record(std::forward<Event>(event));
    });
    display_timer->update();
};

}   // namespace window
