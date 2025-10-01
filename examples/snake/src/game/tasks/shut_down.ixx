export module snake.game.tasks.shut_down;

import ddge.modules.execution.v2.TaskBuilder;

namespace game {

export [[nodiscard]]
auto shut_down() -> ddge::exec::v2::TaskBuilder<void>;

}   // namespace game
