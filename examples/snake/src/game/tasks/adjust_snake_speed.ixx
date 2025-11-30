export module snake.game.tasks.adjust_snake_speed;

import ddge.modules.scheduler.TaskBuilder;

namespace game::tasks {

export [[nodiscard]]
auto adjust_snake_speed() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace game::tasks
