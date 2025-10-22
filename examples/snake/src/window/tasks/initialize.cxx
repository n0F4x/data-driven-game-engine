module snake.window.tasks.initialize;

import ddge.modules.time.FixedTimer;

import ddge.modules.execution.accessors.resources;
import ddge.modules.execution.providers.ResourceProvider;
import ddge.modules.execution.v2.primitives.as_task;

import snake.window.DisplayTimer;
import snake.window.SecondTimer;

using namespace ddge::exec::accessors;

auto window::tasks::initialize()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::as_task(
        +[](const resources::Resource<DisplayTimer> display_timer,
            const resources::Resource<SecondTimer>  second_timer) -> void {
            display_timer->reset();
            second_timer->reset();
        }   //
    );
}
