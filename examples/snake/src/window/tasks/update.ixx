export module snake.window.tasks.update;

import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.TaskBlueprint;

namespace window::tasks {

export [[nodiscard]]
auto update()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>;

}   // namespace window::tasks
