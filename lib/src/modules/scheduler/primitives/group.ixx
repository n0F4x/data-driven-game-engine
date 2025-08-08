module;

#include <concepts>
#include <utility>

export module modules.scheduler.primitives.group;

import modules.scheduler.Nexus;
import modules.scheduler.as_task_builder_t;
import modules.scheduler.converts_to_task_builder_c;
import modules.scheduler.Task;
import modules.scheduler.TaskBuilder;
import modules.scheduler.wrap_as_builder;

namespace modules::scheduler {

inline namespace primitives {

export template <converts_to_task_builder_c... TaskBuilders_T>
    requires(
        std::same_as<typename as_task_builder_t<TaskBuilders_T>::Result, void>
        && ...
    )
[[nodiscard]]
auto group(TaskBuilders_T&&... task_builders) -> TaskBuilder<void>;

}   // namespace primitives

}   // namespace modules::scheduler

template <modules::scheduler::converts_to_task_builder_c... TaskBuilders_T>
    requires(
        std::same_as<typename modules::scheduler::as_task_builder_t<TaskBuilders_T>::Result, void>
        && ...
    )
auto modules::scheduler::primitives::group(TaskBuilders_T&&... task_builders)
    -> TaskBuilder<void>
{
    return TaskBuilder<void>{
        [... wrapped_task_builders = wrap_as_builder(
             std::forward<TaskBuilders_T>(task_builders)
         )](Nexus& nexus) -> Task<void> {
            return [... tasks = wrapped_task_builders.build(nexus)] mutable -> void {
                (tasks(), ...);
            };
        }
    };
}
