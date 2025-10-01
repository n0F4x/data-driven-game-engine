module snake.window.tasks.initialize;

import ddge.modules.time.FixedTimer;

import ddge.modules.execution.accessors.resources;
import ddge.modules.execution.providers.ResourceProvider;
import ddge.modules.execution.v2.primitives.as_task;

import snake.window.DisplayTimer;

using namespace ddge::exec::accessors;

auto window::initialize() -> ddge::exec::v2::TaskBuilder<void>
{
    return ddge::exec::v2::as_task(
        +[](const resources::Resource<DisplayTimer> display_timer) -> void {
            display_timer->reset();
        }   //
    );
}
