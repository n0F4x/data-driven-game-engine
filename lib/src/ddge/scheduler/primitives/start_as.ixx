module;

#include <utility>

export module ddge.scheduler.primitives.start_as;

import ddge.scheduler.ScheduleBuilder;
import ddge.scheduler.TaskBuilder;

namespace ddge::scheduler {

export [[nodiscard]]
auto start_as(TaskBuilder<void>&& task_builder) -> ScheduleBuilder
{
    return ScheduleBuilder{ std::move(task_builder) };
}

}   // namespace ddge::scheduler
