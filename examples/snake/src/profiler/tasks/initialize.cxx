module snake.profiler.tasks.initialize;

import ddge.modules.exec.accessors.resources;
import ddge.modules.exec.primitives.as_task;

import snake.profiler.SecondTimer;

using namespace ddge::exec::accessors;

auto profiler::tasks::initialize()
    -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>
{
    return ddge::exec::as_task(
        +[](const resources::Resource<SecondTimer> display_timer) -> void {
            display_timer->reset();
        }   //
    );
}
