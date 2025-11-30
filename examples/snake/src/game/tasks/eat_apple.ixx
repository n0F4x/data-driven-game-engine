export module snake.game.tasks.eat_apple;

import ddge.modules.scheduler.TaskBuilder;

namespace game::tasks {

export [[nodiscard]]
auto eat_apple() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace game::tasks
