export module snake.game.tasks.color_cells;

import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.TaskBlueprint;

namespace game::tasks {

export [[nodiscard]]
auto color_cells()
    -> ddge::scheduler::TaskBlueprint<void, ddge::scheduler::Cardinality::eSingle>;

}   // namespace game::tasks
