export module snake.game.tasks.initialize;

import ddge.modules.scheduler.TaskBuilder;

namespace game::tasks {

export [[nodiscard]]
auto initialize() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace game::tasks
