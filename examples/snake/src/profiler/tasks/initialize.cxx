module snake.profiler.tasks.initialize;

import ddge.modules.exec.accessors.resources;
import ddge.modules.exec.providers.ResourceProvider;
import ddge.modules.exec.v2.primitives.as_task;

import snake.profiler.SecondTimer;

using namespace ddge::exec::accessors;

auto profiler::tasks::initialize()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>
{
    return ddge::exec::v2::as_task(
        +[](const resources::Resource<SecondTimer> display_timer) -> void {
            display_timer->reset();
        }   //
    );
}
