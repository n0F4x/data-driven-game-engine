export module snake.game.tasks.update;

import ddge.modules.scheduler.TaskBuilder;

namespace game::tasks {

export [[nodiscard]]
auto update() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace game::tasks
