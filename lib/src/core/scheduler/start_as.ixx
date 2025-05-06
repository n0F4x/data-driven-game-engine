module;

#include <utility>

export module core.scheduler.start_as;

import core.scheduler.as_task_builder;
import core.scheduler.concepts.converts_to_task_builder_c;
import core.scheduler.task_builders.ScheduleBuilder;

namespace core::scheduler {

export template <converts_to_task_builder_c TaskBuilder_T>
[[nodiscard]]
constexpr auto start_as(TaskBuilder_T&& task_builder)
    -> ScheduleBuilder<as_task_builder_t<TaskBuilder_T>>;

}   // namespace core::scheduler

template <core::scheduler::converts_to_task_builder_c TaskBuilder_T>
constexpr auto core::scheduler::start_as(TaskBuilder_T&& task_builder)
    -> ScheduleBuilder<as_task_builder_t<TaskBuilder_T>>
{
    return ScheduleBuilder<as_task_builder_t<TaskBuilder_T>>{
        std::forward<TaskBuilder_T>(task_builder)
    };
}
