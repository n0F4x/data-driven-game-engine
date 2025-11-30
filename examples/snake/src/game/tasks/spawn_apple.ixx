export module snake.game.tasks.spawn_apple;

import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.TaskBlueprint;

namespace game::tasks {

export [[nodiscard]]
auto spawn_apple()
    -> ddge::scheduler::TaskBlueprint<void, ddge::scheduler::Cardinality::eSingle>;

}   // namespace game::tasks
