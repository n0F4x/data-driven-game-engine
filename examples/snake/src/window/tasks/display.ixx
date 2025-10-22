export module snake.window.tasks.display;

import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.TaskBlueprint;

namespace window {

export [[nodiscard]]
auto display()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>;

}   // namespace window
