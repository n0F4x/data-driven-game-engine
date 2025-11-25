export module snake.game.tasks.game_is_running;

import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.TaskBlueprint;

namespace game::tasks {

export [[nodiscard]]
auto game_is_running()
    -> ddge::exec::v2::TaskBlueprint<bool, ddge::exec::v2::Cardinality::eSingle>;

}   // namespace game::tasks
