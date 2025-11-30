module;

#include <utility>

export module ddge.modules.scheduler.primitives.start_as;

import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.ScheduleBuilder;
import ddge.modules.scheduler.TaskBlueprint;

namespace ddge::scheduler {

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

}   // namespace ddge::scheduler
