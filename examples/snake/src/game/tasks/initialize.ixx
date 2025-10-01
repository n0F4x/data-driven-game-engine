export module snake.game.tasks.initialize;

import ddge.modules.execution.v2.TaskBuilder;

namespace game {

export [[nodiscard]]
auto initialize() -> ddge::exec::v2::TaskBuilder<void>;

}   // namespace game
