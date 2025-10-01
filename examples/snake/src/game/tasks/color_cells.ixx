export module snake.game.tasks.color_cells;

import ddge.modules.execution.v2.TaskBuilder;

namespace game {

export [[nodiscard]]
auto color_cells() -> ddge::exec::v2::TaskBuilder<void>;

}   // namespace game
