export module snake.window.initialize;

import core.scheduler;
import core.time.FixedTimer;

import extensions.scheduler.accessors.resources;

import snake.window.DisplayTimer;
import snake.window.open_window;

using namespace extensions::scheduler::accessors;

namespace window {

export inline constexpr auto initialize = core::scheduler::group(
    open_window,
    [](const resources::Resource<DisplayTimer> display_timer) {
        display_timer->reset();
    }
);

}   // namespace window
