export module snake.window.tasks.initialize;

import ddge.modules.execution.v2.TaskBuilder;

namespace window {

export [[nodiscard]]
auto initialize() -> ddge::exec::v2::TaskBuilder<void>;

}   // namespace window
