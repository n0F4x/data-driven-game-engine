export module snake.game.tasks.move_snake;

import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.TaskBlueprint;

namespace game::tasks {

export [[nodiscard]]
auto move_snake()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>;

}   // namespace game::tasks
