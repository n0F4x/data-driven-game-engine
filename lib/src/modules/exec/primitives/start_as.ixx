module;

#include <utility>

export module ddge.modules.exec.primitives.start_as;

import ddge.modules.exec.Cardinality;
import ddge.modules.exec.ScheduleBuilder;
import ddge.modules.exec.TaskBlueprint;

namespace ddge::exec {

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

}   // namespace ddge::exec
