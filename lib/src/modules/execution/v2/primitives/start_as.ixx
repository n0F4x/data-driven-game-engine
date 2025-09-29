module;

#include <utility>

export module ddge.modules.execution.v2.primitives.start_as;

import ddge.modules.execution.v2.ScheduleBuilder;
import ddge.modules.execution.v2.TaskBuilder;

namespace ddge::exec::v2 {

export auto start_as(TaskBuilder<void>&& task_builder) -> ScheduleBuilder
{
    return ScheduleBuilder{ std::move(task_builder) };
}

}   // namespace ddge::exec::v2
