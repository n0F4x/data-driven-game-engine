export module snake.profiler.tasks.update;

import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.TaskBlueprint;

namespace profiler::tasks {

export [[nodiscard]]
auto update()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>;

}   // namespace profiler::tasks
