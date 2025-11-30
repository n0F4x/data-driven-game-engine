export module snake.game.tasks.eat_apple;

import ddge.modules.exec.Cardinality;
import ddge.modules.exec.TaskBlueprint;

namespace game::tasks {

export [[nodiscard]]
auto eat_apple() -> ddge::exec::TaskBlueprint<void, ddge::exec::Cardinality::eSingle>;

}   // namespace game::tasks
