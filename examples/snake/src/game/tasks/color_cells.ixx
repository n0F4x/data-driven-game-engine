export module snake.game.tasks.color_cells;

import ddge.modules.exec.Cardinality;
import ddge.modules.exec.TaskBlueprint;

namespace game::tasks {

export [[nodiscard]]
auto color_cells() -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>;

}   // namespace game::tasks
