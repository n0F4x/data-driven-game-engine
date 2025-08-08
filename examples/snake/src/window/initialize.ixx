export module snake.window.initialize;

import core.time.FixedTimer;

import core.scheduler.accessors.resources;

import snake.window.DisplayTimer;

using namespace core::scheduler::accessors;

namespace window {

export auto initialize(resources::Resource<DisplayTimer> display_timer) -> void;

}   // namespace window

module :private;

auto window::initialize(const resources::Resource<DisplayTimer> display_timer) -> void
{
    display_timer->reset();
}
