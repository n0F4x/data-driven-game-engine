export module snake.window.initialize;

import ddge.modules.time.FixedTimer;

import ddge.modules.execution.accessors.resources;

import snake.window.DisplayTimer;

using namespace ddge::exec::accessors;

namespace window {

export auto initialize(resources::Resource<DisplayTimer> display_timer) -> void;

}   // namespace window

module :private;

auto window::initialize(const resources::Resource<DisplayTimer> display_timer) -> void
{
    display_timer->reset();
}
