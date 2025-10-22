export module snake.game.tasks.shut_down;

import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.TaskBlueprint;

namespace game::tasks {

export [[nodiscard]]
auto shut_down()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>;

}   // namespace game::tasks
