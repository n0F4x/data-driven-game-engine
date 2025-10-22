export module snake.window.tasks.window_should_close;

import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.TaskBlueprint;

namespace window::tasks {

export auto window_should_close()
    -> ddge::exec::v2::TaskBlueprint<bool, ddge::exec::v2::Cardinality::eSingle>;

}   // namespace window::tasks
