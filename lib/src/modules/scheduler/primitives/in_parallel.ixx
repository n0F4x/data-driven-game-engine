module;

#include <concepts>
#include <utility>

export module ddge.modules.scheduler.primitives.in_parallel;

import ddge.modules.scheduler.Nexus;
import ddge.modules.scheduler.as_task_builder_t;
import ddge.modules.scheduler.converts_to_task_builder_c;
import ddge.modules.scheduler.Task;
import ddge.modules.scheduler.TaskBuilder;
import ddge.modules.scheduler.wrap_as_builder;

namespace ddge::scheduler {

inline namespace primitives {

export template <converts_to_task_builder_c... TaskBuilders_T>
    requires(std::same_as<typename as_task_builder_t<TaskBuilders_T>::Result, void> && ...)
[[nodiscard]]
auto in_parallel(TaskBuilders_T&&... task_builders) -> TaskBuilder<void>;

}   // namespace primitives

}   // namespace ddge::scheduler

template <ddge::scheduler::converts_to_task_builder_c... TaskBuilders_T>
    requires(
        std::same_as<typename ddge::scheduler::as_task_builder_t<TaskBuilders_T>::Result, void>
        && ...
    )
auto ddge::scheduler::primitives::in_parallel(TaskBuilders_T&&... task_builders)
    -> TaskBuilder<void>
{
    return TaskBuilder<void>{ [... wrapped_task_builders = wrap_as_builder(
                                   std::forward<TaskBuilders_T>(task_builders)
                               )](Nexus& nexus) -> Task<void> {
        return
            [... tasks = wrapped_task_builders.build(nexus)] -> void { (tasks(), ...); };
    } };
}
