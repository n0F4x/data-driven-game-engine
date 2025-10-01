export module snake.window.tasks.close_window;

import ddge.modules.execution.v2.TaskBuilder;

namespace window {

export [[nodiscard]]
auto close_window() -> ddge::exec::v2::TaskBuilder<void>;

}   // namespace window
