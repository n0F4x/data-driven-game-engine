export module snake.profiler.tasks.initialize;

import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.TaskBlueprint;

namespace profiler::tasks {

export [[nodiscard]]
auto initialize()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>;

}   // namespace profiler::tasks
