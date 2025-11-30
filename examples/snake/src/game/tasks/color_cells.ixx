export module snake.game.tasks.color_cells;

import ddge.modules.scheduler.TaskBuilder;

namespace game::tasks {

export [[nodiscard]]
auto color_cells() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace game::tasks
