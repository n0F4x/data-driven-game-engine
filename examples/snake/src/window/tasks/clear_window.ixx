export module snake.window.tasks.clear_window;

import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.TaskBlueprint;

namespace window::tasks {

export [[nodiscard]]
auto clear_window()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>;

}   // namespace window::tasks
