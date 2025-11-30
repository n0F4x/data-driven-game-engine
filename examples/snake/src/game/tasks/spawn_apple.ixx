export module snake.game.tasks.spawn_apple;

import ddge.modules.scheduler.TaskBuilder;

namespace game::tasks {

export [[nodiscard]]
auto spawn_apple() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace game::tasks
