export module snake.window.tasks.initialize;

import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.TaskBlueprint;

namespace window::tasks {

export [[nodiscard]]
auto initialize()
    -> ddge::scheduler::TaskBlueprint<void, ddge::scheduler::Cardinality::eSingle>;

}   // namespace window::tasks
