export module snake.window.tasks.display;

import ddge.modules.exec.Cardinality;
import ddge.modules.exec.TaskBlueprint;

namespace window::tasks {

export [[nodiscard]]
auto display() -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>;

}   // namespace window::tasks
