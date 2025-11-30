export module snake.window.tasks.close_window;

import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.TaskBlueprint;

namespace window::tasks {

export [[nodiscard]]
auto close_window()
    -> ddge::scheduler::TaskBlueprint<void, ddge::scheduler::Cardinality::eSingle>;

}   // namespace window::tasks
