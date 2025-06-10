export module snake.window.initialize;

import core.scheduler;
import core.time.FixedTimer;

import extensions.scheduler.accessors.resources;

import snake.window.display_rate;
import snake.window.open_window;

using namespace extensions::scheduler::accessors;

namespace window {

export inline constexpr auto initialize = core::scheduler::group(
    open_window,
    [](const resources::Ref<core::time::FixedTimer<display_rate>> display_timer) {
        display_timer->reset();
    }
);

}   // namespace window
