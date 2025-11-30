export module snake.game.tasks.draw;

import ddge.modules.scheduler.TaskBuilder;

namespace game::tasks {

export [[nodiscard]]
auto draw() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace game::tasks
