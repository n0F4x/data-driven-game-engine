export module snake.profiler.tasks.update;

import ddge.modules.scheduler.TaskBuilder;

namespace profiler::tasks {

export [[nodiscard]]
auto update() -> ddge::scheduler::TaskBuilder<void>;

}   // namespace profiler::tasks
