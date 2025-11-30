export module snake.game.tasks.move_snake;

import ddge.modules.scheduler.TaskBuilder;

namespace game::tasks {

export [[nodiscard]]
auto move_snake() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace game::tasks
