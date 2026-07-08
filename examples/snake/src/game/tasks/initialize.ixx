export module snake.game.tasks.initialize;

import ddge.scheduler.TaskBuilder;

namespace game::tasks {

export [[nodiscard]]
auto initialize() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace game::tasks
