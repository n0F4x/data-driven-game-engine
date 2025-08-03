module;

#include <utility>

export module extensions.scheduler.primitives.start_as;

import core.scheduler.as_task_builder_t;
import core.scheduler.converts_to_task_builder_c;
import core.scheduler.SchedulerBuilder;
import core.scheduler.wrap_as_builder;

namespace extensions::scheduler {

inline namespace primitives {

export template <core::scheduler::converts_to_task_builder_c TaskBuilder_T>
    requires std::
        same_as<typename core::scheduler::as_task_builder_t<TaskBuilder_T>::Result, void>
    [[nodiscard]]
    auto start_as(TaskBuilder_T&& task_builder) -> core::scheduler::SchedulerBuilder;

}   // namespace primitives

}   // namespace extensions::scheduler

template <core::scheduler::converts_to_task_builder_c TaskBuilder_T>
    requires std::
        same_as<typename core::scheduler::as_task_builder_t<TaskBuilder_T>::Result, void>
    auto extensions::scheduler::primitives::start_as(TaskBuilder_T&& task_builder)
        -> core::scheduler::SchedulerBuilder
{
    return core::scheduler::SchedulerBuilder{
        core::scheduler::wrap_as_builder(std::forward<TaskBuilder_T>(task_builder))
    };
}
