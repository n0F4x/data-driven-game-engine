export module snake.window.tasks.update;

import ddge.modules.scheduler.TaskBuilder;

namespace window::tasks {

export [[nodiscard]]
auto update() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace window::tasks
