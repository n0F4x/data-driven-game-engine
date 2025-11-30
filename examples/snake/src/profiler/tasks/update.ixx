export module snake.profiler.tasks.update;

import ddge.modules.exec.Cardinality;
import ddge.modules.exec.TaskBlueprint;

namespace profiler::tasks {

export [[nodiscard]]
auto update() -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>;

}   // namespace profiler::tasks
