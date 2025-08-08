module;

#include <utility>

export module modules.scheduler.primitives.start_as;

import modules.scheduler.as_task_builder_t;
import modules.scheduler.converts_to_task_builder_c;
import modules.scheduler.SchedulerBuilder;
import modules.scheduler.wrap_as_builder;

namespace modules::scheduler {

inline namespace primitives {

export template <converts_to_task_builder_c TaskBuilder_T>
    requires std::same_as<typename as_task_builder_t<TaskBuilder_T>::Result, void>
[[nodiscard]]
auto start_as(TaskBuilder_T&& task_builder) -> SchedulerBuilder;

}   // namespace primitives

}   // namespace modules::scheduler

template <modules::scheduler::converts_to_task_builder_c TaskBuilder_T>
    requires std::
        same_as<typename modules::scheduler::as_task_builder_t<TaskBuilder_T>::Result, void>
    auto modules::scheduler::primitives::start_as(TaskBuilder_T&& task_builder)
        -> SchedulerBuilder
{
    return SchedulerBuilder{ wrap_as_builder(std::forward<TaskBuilder_T>(task_builder)) };
}
