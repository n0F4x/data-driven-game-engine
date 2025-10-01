export module snake.window.tasks.window_should_close;

import ddge.modules.execution.v2.TaskBuilder;

namespace window {

export auto window_should_close() -> ddge::exec::v2::TaskBuilder<bool>;

}   // namespace window
