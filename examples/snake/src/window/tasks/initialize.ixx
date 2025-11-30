export module snake.window.tasks.initialize;

import ddge.modules.exec.Cardinality;
import ddge.modules.exec.TaskBlueprint;

namespace window::tasks {

export [[nodiscard]]
auto initialize() -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>;

}   // namespace window::tasks
