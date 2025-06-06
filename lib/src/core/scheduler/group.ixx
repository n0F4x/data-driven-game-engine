module;

#include <utility>

export module core.scheduler.group;

import core.scheduler.as_task_builder;
import core.scheduler.concepts.converts_to_task_builder_c;
import core.scheduler.task_builders.GroupedTaskBuilder;

namespace core::scheduler {

export template <converts_to_task_builder_c... TaskBuilders_T>
[[nodiscard]]
constexpr auto group(TaskBuilders_T&&... task_builders)
    -> GroupedTaskBuilder<as_task_builder_t<TaskBuilders_T>...>;

}   // namespace core::scheduler

template <core::scheduler::converts_to_task_builder_c... TaskBuilders_T>
constexpr auto core::scheduler::group(TaskBuilders_T&&... task_builders)
    -> GroupedTaskBuilder<as_task_builder_t<TaskBuilders_T>...>
{
    return GroupedTaskBuilder<as_task_builder_t<TaskBuilders_T>...>{
        std::forward<TaskBuilders_T>(task_builders)...
    };
}
