export module snake.game.tasks.game_is_running;

import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.TaskBlueprint;

namespace game::tasks {

export [[nodiscard]]
auto game_is_running()
    -> ddge::scheduler::TaskBlueprint<bool, ddge::scheduler::Cardinality::eSingle>;

}   // namespace game::tasks
