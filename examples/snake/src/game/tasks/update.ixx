export module snake.game.tasks.update;

import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.TaskBlueprint;

namespace game::tasks {

export [[nodiscard]]
auto update()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>;

}   // namespace game::tasks
