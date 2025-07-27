export module snake.window.initialize;

import core.scheduler;
import core.time.FixedTimer;

import extensions.scheduler.accessors.resources;

import snake.window.DisplayTimer;

using namespace extensions::scheduler::accessors;

namespace window {

export auto initialize(resources::Resource<DisplayTimer> display_timer) -> void;

}   // namespace window

module :private;

auto window::initialize(const resources::Resource<DisplayTimer> display_timer) -> void
{
    display_timer->reset();
}
