export module demo.window.initialize;

import core.scheduler;
import core.time.FixedTimer;

import extensions.scheduler.accessors.resources;

import demo.window.display_rate;
import demo.window.open_window;

using namespace extensions::scheduler::accessors;

namespace window {

export inline constexpr auto initialize = core::scheduler::group(
    open_window,
    [](const resources::Ref<core::time::FixedTimer<display_rate>> display_timer) {
        display_timer->reset();
    }
);

}   // namespace window
