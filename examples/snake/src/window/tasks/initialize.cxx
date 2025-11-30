module snake.window.tasks.initialize;

import ddge.modules.time.FixedTimer;

import ddge.modules.exec.accessors.resources;
import ddge.modules.exec.primitives.as_task;

import snake.window.DisplayTimer;
import snake.window.SecondTimer;

using namespace ddge::exec::accessors;

auto window::tasks::initialize()
    -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>
{
    return ddge::exec::as_task(
        +[](const resources::Resource<DisplayTimer> display_timer,
            const resources::Resource<SecondTimer>  second_timer) -> void {
            display_timer->reset();
            second_timer->reset();
        }   //
    );
}
