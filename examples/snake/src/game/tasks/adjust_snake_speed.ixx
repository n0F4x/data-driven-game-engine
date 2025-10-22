export module snake.game.tasks.adjust_snake_speed;

import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.TaskBlueprint;

namespace game {

export [[nodiscard]]
auto adjust_snake_speed()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>;

}   // namespace game
