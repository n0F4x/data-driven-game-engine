export module snake.window.tasks.display;

import ddge.modules.execution.v2.TaskBuilder;

namespace window {

export [[nodiscard]]
auto display() -> ddge::exec::v2::TaskBuilder<void>;

}   // namespace window
