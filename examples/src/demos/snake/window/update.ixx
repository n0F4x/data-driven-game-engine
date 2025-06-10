export module snake.window.update;

import core.scheduler;
import core.time.FixedTimer;

import extensions.scheduler.accessors.resources;

import snake.window.display_rate;
import snake.window.Window;

using namespace extensions::scheduler::accessors;

namespace window {

export inline constexpr auto update =
    [](const resources::Ref<Window>                               window,
       const window::EventRecorder&                               window_event_recorder,
       const resources::Ref<core::time::FixedTimer<display_rate>> display_timer)   //
{
    window->record_events(window_event_recorder);
    display_timer->update();
};

}   // namespace window
