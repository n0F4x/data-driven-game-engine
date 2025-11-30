module;

#include <utility>

export module ddge.modules.scheduler.primitives.start_as;

import ddge.modules.scheduler.ScheduleBuilder;
import ddge.modules.scheduler.TaskBuilder;

namespace ddge::scheduler {

export [[nodiscard]]
auto start_as(TaskBuilder<void>&& task_builder) -> ScheduleBuilder
{
    return ScheduleBuilder{ std::move(task_builder) };
}

}   // namespace ddge::scheduler
