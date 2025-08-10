module;

#include <utility>

export module ddge.modules.execution.primitives.start_as;

import ddge.modules.execution.as_task_builder_t;
import ddge.modules.execution.converts_to_task_builder_c;
import ddge.modules.execution.SchedulerBuilder;
import ddge.modules.execution.wrap_as_builder;

namespace ddge::exec {

inline namespace primitives {

export template <converts_to_task_builder_c TaskBuilder_T>
    requires std::same_as<typename as_task_builder_t<TaskBuilder_T>::Result, void>
[[nodiscard]]
auto start_as(TaskBuilder_T&& task_builder) -> SchedulerBuilder;

}   // namespace primitives

}   // namespace ddge::exec

template <ddge::exec::converts_to_task_builder_c TaskBuilder_T>
    requires std::
        same_as<typename ddge::exec::as_task_builder_t<TaskBuilder_T>::Result, void>
    auto ddge::exec::primitives::start_as(TaskBuilder_T&& task_builder)
        -> SchedulerBuilder
{
    return SchedulerBuilder{ wrap_as_builder(std::forward<TaskBuilder_T>(task_builder)) };
}
