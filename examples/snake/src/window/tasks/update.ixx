export module snake.window.tasks.update;

import ddge.modules.exec.Cardinality;
import ddge.modules.exec.TaskBlueprint;

namespace window::tasks {

export [[nodiscard]]
auto update() -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>;

}   // namespace window::tasks
