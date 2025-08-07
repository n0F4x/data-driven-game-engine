module;

#include <concepts>
#include <utility>

export module extensions.scheduler.primitives.group;

import core.scheduler.Nexus;
import core.scheduler.as_task_builder_t;
import core.scheduler.converts_to_task_builder_c;
import core.scheduler.Task;
import core.scheduler.TaskBuilder;
import core.scheduler.wrap_as_builder;

namespace extensions::scheduler {

inline namespace primitives {

export template <core::scheduler::converts_to_task_builder_c... TaskBuilders_T>
    requires(
        std::same_as<typename core::scheduler::as_task_builder_t<TaskBuilders_T>::Result, void>
        && ...
    )
[[nodiscard]]
auto group(TaskBuilders_T&&... task_builders) -> core::scheduler::TaskBuilder<void>;

}   // namespace primitives

}   // namespace extensions::scheduler

template <core::scheduler::converts_to_task_builder_c... TaskBuilders_T>
    requires(
        std::same_as<typename core::scheduler::as_task_builder_t<TaskBuilders_T>::Result, void>
        && ...
    )
auto extensions::scheduler::primitives::group(TaskBuilders_T&&... task_builders)
    -> core::scheduler::TaskBuilder<void>
{
    return core::scheduler::TaskBuilder<void>{
        [... wrapped_task_builders = core::scheduler::wrap_as_builder(
             std::forward<TaskBuilders_T>(task_builders)
         )](core::scheduler::Nexus& nexus) -> core::scheduler::Task<void> {
            return [... tasks = wrapped_task_builders.build(nexus)] mutable -> void {
                (tasks(), ...);
            };
        }
    };
}
