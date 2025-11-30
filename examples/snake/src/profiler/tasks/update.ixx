export module snake.profiler.tasks.update;

import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.TaskBlueprint;

namespace profiler::tasks {

export [[nodiscard]]
auto update()
    -> ddge::scheduler::TaskBlueprint<void, ddge::scheduler::Cardinality::eSingle>;

}   // namespace profiler::tasks
