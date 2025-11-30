export module snake.game.tasks.draw;

import ddge.modules.exec.Cardinality;
import ddge.modules.exec.TaskBlueprint;

namespace game::tasks {

export [[nodiscard]]
auto draw() -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>;

}   // namespace game::tasks
