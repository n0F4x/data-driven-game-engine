module;

#include <utility>

export module ddge.modules.scheduler.primitives.start_as;

import ddge.modules.scheduler.as_task_builder_t;
import ddge.modules.scheduler.converts_to_task_builder_c;
import ddge.modules.scheduler.SchedulerBuilder;
import ddge.modules.scheduler.wrap_as_builder;

namespace ddge::scheduler {

inline namespace primitives {

export template <converts_to_task_builder_c TaskBuilder_T>
    requires std::same_as<typename as_task_builder_t<TaskBuilder_T>::Result, void>
[[nodiscard]]
auto start_as(TaskBuilder_T&& task_builder) -> SchedulerBuilder;

}   // namespace primitives

}   // namespace ddge::scheduler

template <ddge::scheduler::converts_to_task_builder_c TaskBuilder_T>
    requires std::
        same_as<typename ddge::scheduler::as_task_builder_t<TaskBuilder_T>::Result, void>
    auto ddge::scheduler::primitives::start_as(TaskBuilder_T&& task_builder)
        -> SchedulerBuilder
{
    return SchedulerBuilder{ wrap_as_builder(std::forward<TaskBuilder_T>(task_builder)) };
}
