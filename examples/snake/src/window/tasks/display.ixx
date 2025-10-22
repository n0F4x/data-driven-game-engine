export module snake.window.tasks.display;

import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.TaskBlueprint;

namespace window::tasks {

export [[nodiscard]]
auto display()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>;

}   // namespace window::tasks
