module;

#include <utility>

export module ddge.modules.execution.v2.primitives.start_as;

import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.ScheduleBuilder;
import ddge.modules.execution.v2.TaskBlueprint;

namespace ddge::exec::v2 {

export auto start_as(TaskBlueprint<void, Cardinality::eSingle>&& task_blueprint)
    -> ScheduleBuilder
{
    return ScheduleBuilder{ std::move(task_blueprint) };
}

export auto start_as(TaskBlueprint<void, Cardinality::eMulti>&& task_blueprint)
    -> ScheduleBuilder
{
    return ScheduleBuilder{ std::move(task_blueprint) };
}

}   // namespace ddge::exec::v2
