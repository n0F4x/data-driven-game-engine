export module snake.window.tasks.clear_window;

import ddge.modules.execution.v2.TaskBuilder;

namespace window {

export [[nodiscard]]
auto clear_window() -> ddge::exec::v2::TaskBuilder<void>;

}   // namespace window
