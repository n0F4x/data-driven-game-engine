export module snake.window.tasks.window_should_close;

import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.TaskBlueprint;

namespace window::tasks {

export auto window_should_close()
    -> ddge::scheduler::TaskBlueprint<bool, ddge::scheduler::Cardinality::eSingle>;

}   // namespace window::tasks
