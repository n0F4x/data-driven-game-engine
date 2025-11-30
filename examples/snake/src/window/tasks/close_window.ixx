export module snake.window.tasks.close_window;

import ddge.modules.exec.Cardinality;
import ddge.modules.exec.TaskBlueprint;

namespace window::tasks {

export [[nodiscard]]
auto close_window() -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>;

}   // namespace window::tasks
