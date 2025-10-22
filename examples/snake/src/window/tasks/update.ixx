export module snake.window.tasks.update;

import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.TaskBlueprint;

namespace window {

export [[nodiscard]]
auto update()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>;

}   // namespace window
