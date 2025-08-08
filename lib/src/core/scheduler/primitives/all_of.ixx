module;

#include <concepts>
#include <utility>

export module core.scheduler.primitives.all_of;

import core.scheduler.Nexus;
import core.scheduler.as_task_builder_t;
import core.scheduler.converts_to_task_builder_c;
import core.scheduler.Task;
import core.scheduler.TaskBuilder;
import core.scheduler.wrap_as_builder;

namespace core::scheduler {

inline namespace primitives {

export template <core::scheduler::converts_to_task_builder_c... TaskBuilders_T>
    requires(
        std::same_as<typename core::scheduler::as_task_builder_t<TaskBuilders_T>::Result, bool>
        && ...
    )
[[nodiscard]]
auto all_of(TaskBuilders_T&&... task_builders) -> core::scheduler::TaskBuilder<bool>;

}   // namespace primitives

}   // namespace core::scheduler

template <core::scheduler::converts_to_task_builder_c... TaskBuilders_T>
    requires(
        std::same_as<typename core::scheduler::as_task_builder_t<TaskBuilders_T>::Result, bool>
        && ...
    )
auto core::scheduler::primitives::all_of(TaskBuilders_T&&... task_builders)
    -> core::scheduler::TaskBuilder<bool>
{
    return core::scheduler::TaskBuilder<bool>{
        [... wrapped_task_builders = core::scheduler::wrap_as_builder(
             std::forward<TaskBuilders_T>(task_builders)
         )](core::scheduler::Nexus& nexus) -> core::scheduler::Task<bool> {
            return [... tasks = wrapped_task_builders.build(nexus)] mutable -> bool {
                return (tasks() && ...);
            };
        }
    };
}
