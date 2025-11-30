export module snake.window.tasks.window_should_close;

import ddge.modules.exec.Cardinality;
import ddge.modules.exec.TaskBlueprint;

namespace window::tasks {

export auto window_should_close()
    -> ddge::exec::TaskBlueprint<bool, ddge::exec::Cardinality::eSingle>;

}   // namespace window::tasks
