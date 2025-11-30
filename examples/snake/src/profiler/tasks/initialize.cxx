module snake.profiler.tasks.initialize;

import ddge.modules.scheduler.accessors.resources;
import ddge.modules.scheduler.primitives.as_task;

import snake.profiler.SecondTimer;

using namespace ddge::scheduler::accessors;

auto profiler::tasks::initialize()
    -> ddge::scheduler::TaskBlueprint<void, ddge::scheduler::Cardinality::eSingle>
{
    return ddge::scheduler::as_task(
        +[](const resources::Resource<SecondTimer> display_timer) -> void {
            display_timer->reset();
        }   //
    );
}
