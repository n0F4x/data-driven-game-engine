export module snake.window.tasks.update;

import ddge.modules.execution.v2.TaskBuilder;

namespace window {

export [[nodiscard]]
auto update() -> ddge::exec::v2::TaskBuilder<void>;

}   // namespace window
