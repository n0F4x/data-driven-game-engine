module;

#include <concepts>
#include <utility>

export module ddge.modules.execution.primitives.group;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.as_task_builder_t;
import ddge.modules.execution.converts_to_task_builder_c;
import ddge.modules.execution.Task;
import ddge.modules.execution.TaskBuilder;
import ddge.modules.execution.wrap_as_builder;

namespace ddge::exec {

inline namespace primitives {

export template <converts_to_task_builder_c... TaskBuilders_T>
    requires(std::same_as<typename as_task_builder_t<TaskBuilders_T>::Result, void> && ...)
[[nodiscard]]
auto group(TaskBuilders_T&&... task_builders) -> TaskBuilder<void>;

}   // namespace primitives

}   // namespace ddge::exec

template <ddge::exec::converts_to_task_builder_c... TaskBuilders_T>
    requires(
        std::same_as<typename ddge::exec::as_task_builder_t<TaskBuilders_T>::Result, void>
        && ...
    )
auto ddge::exec::primitives::group(TaskBuilders_T&&... task_builders)
    -> TaskBuilder<void>
{
    return TaskBuilder<void>{ [... wrapped_task_builders = wrap_as_builder(
                                   std::forward<TaskBuilders_T>(task_builders)
                               )](Nexus& nexus) -> Task<void> {
        return [... tasks = wrapped_task_builders.build(nexus)] mutable -> void {
            (tasks(), ...);
        };
    } };
}
