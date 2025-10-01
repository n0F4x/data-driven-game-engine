export module snake.game.tasks.update;

import ddge.modules.execution.v2.TaskBuilder;

namespace game {

export [[nodiscard]]
auto update() -> ddge::exec::v2::TaskBuilder<void>;

}   // namespace game
