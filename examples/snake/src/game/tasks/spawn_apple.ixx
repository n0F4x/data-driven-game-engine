export module snake.game.tasks.spawn_apple;

import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.TaskBlueprint;

namespace game::tasks {

export [[nodiscard]]
auto spawn_apple()
    -> ddge::exec::v2::TaskBlueprint<void, ddge::exec::v2::Cardinality::eSingle>;

}   // namespace game::tasks
