export module snake.app.tasks.initialize;

import ddge.modules.scheduler.TaskBuilderGroup;

namespace app::tasks {

export [[nodiscard]]
auto initialize() -> ddge::scheduler::TaskBuilderGroup<void>;

}   // namespace app::tasks
