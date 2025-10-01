export module snake.game.tasks.draw;

import ddge.modules.execution.v2.TaskBuilder;

namespace game {

export [[nodiscard]]
auto draw() -> ddge::exec::v2::TaskBuilder<void>;

}   // namespace game
