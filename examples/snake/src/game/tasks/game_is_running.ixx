export module snake.game.tasks.game_is_running;

import ddge.modules.exec.Cardinality;
import ddge.modules.exec.TaskBlueprint;

namespace game::tasks {

export [[nodiscard]]
auto game_is_running()
    -> ddge::exec::TaskBlueprint<bool, ddge::exec::Cardinality::eSingle>;

}   // namespace game::tasks
