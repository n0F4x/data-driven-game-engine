export module snake.game.tasks.shut_down;

import ddge.modules.scheduler.TaskBuilder;

namespace game::tasks {

export [[nodiscard]]
auto shut_down() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace game::tasks
