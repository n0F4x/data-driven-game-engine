export module snake.game.tasks.game_is_running;

import ddge.scheduler.TaskBuilder;

namespace game::tasks {

export [[nodiscard]]
auto game_is_running() -> ddge::scheduler::TaskBuilder<bool>;

}   // namespace game::tasks
