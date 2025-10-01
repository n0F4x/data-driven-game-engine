export module snake.game.tasks.adjust_snake_speed;

import ddge.modules.execution.v2.TaskBuilder;

namespace game {

export [[nodiscard]]
auto adjust_snake_speed() -> ddge::exec::v2::TaskBuilder<void>;

}   // namespace game
