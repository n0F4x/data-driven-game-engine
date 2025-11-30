export module snake.profiler.tasks.initialize;

import ddge.modules.exec.Cardinality;
import ddge.modules.exec.TaskBlueprint;

namespace profiler::tasks {

export [[nodiscard]]
auto initialize() -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>;

}   // namespace profiler::tasks
