module snake.window.tasks.initialize;

import ddge.time.FixedTimer;

import ddge.scheduler.accessors.resources;
import ddge.scheduler.primitives.as_task;

import snake.window.DisplayTimer;
import snake.window.SecondTimer;

using namespace ddge::scheduler::accessors;

auto window::tasks::initialize() -> ddge::scheduler::TaskBuilder<void>
{
    return ddge::scheduler::as_task(
        +[](const resources::Resource<DisplayTimer> display_timer,
            const resources::Resource<SecondTimer>  second_timer) -> void {
            display_timer->reset();
            second_timer->reset();
        }   //
    );
}
