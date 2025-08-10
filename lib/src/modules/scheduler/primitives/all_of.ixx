module;

#include <concepts>
#include <utility>

export module ddge.modules.scheduler.primitives.all_of;

import ddge.modules.scheduler.Nexus;
import ddge.modules.scheduler.as_task_builder_t;
import ddge.modules.scheduler.converts_to_task_builder_c;
import ddge.modules.scheduler.Task;
import ddge.modules.scheduler.TaskBuilder;
import ddge.modules.scheduler.wrap_as_builder;

namespace ddge::scheduler {

inline namespace primitives {

export template <converts_to_task_builder_c... TaskBuilders_T>
    requires(std::same_as<typename as_task_builder_t<TaskBuilders_T>::Result, bool> && ...)
[[nodiscard]]
auto all_of(TaskBuilders_T&&... task_builders) -> TaskBuilder<bool>;

}   // namespace primitives

}   // namespace ddge::scheduler

template <ddge::scheduler::converts_to_task_builder_c... TaskBuilders_T>
    requires(
        std::same_as<typename ddge::scheduler::as_task_builder_t<TaskBuilders_T>::Result, bool>
        && ...
    )
auto ddge::scheduler::primitives::all_of(TaskBuilders_T&&... task_builders)
    -> TaskBuilder<bool>
{
    return TaskBuilder<bool>{ [... wrapped_task_builders = wrap_as_builder(
                                   std::forward<TaskBuilders_T>(task_builders)
                               )](Nexus& nexus) -> Task<bool> {
        return [... tasks = wrapped_task_builders.build(nexus)] mutable -> bool {
            return (tasks() && ...);
        };
    } };
}
