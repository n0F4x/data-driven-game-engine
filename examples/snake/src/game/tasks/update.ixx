export module snake.game.tasks.update;

import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.TaskBlueprint;

namespace game {

export [[nodiscard]]
auto update()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>;

}   // namespace game
