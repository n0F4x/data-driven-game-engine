export module snake.profiler.tasks.initialize;

import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.TaskBlueprint;

namespace profiler::tasks {

export [[nodiscard]]
auto initialize()
    -> ddge::scheduler::TaskBlueprint<void, ddge::scheduler::Cardinality::eSingle>;

}   // namespace profiler::tasks
