export module snake.game.tasks.move_snake;

import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.TaskBlueprint;

namespace game::tasks {

export [[nodiscard]]
auto move_snake()
    -> ddge::scheduler::TaskBlueprint<void, ddge::scheduler::Cardinality::eSingle>;

}   // namespace game::tasks
