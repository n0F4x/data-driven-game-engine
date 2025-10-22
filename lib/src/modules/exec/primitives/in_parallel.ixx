module;

#include <concepts>
#include <utility>

export module ddge.modules.exec.primitives.in_parallel;

import ddge.modules.exec.Nexus;
import ddge.modules.exec.as_task_builder_t;
import ddge.modules.exec.converts_to_task_builder_c;
import ddge.modules.exec.Task;
import ddge.modules.exec.TaskBuilder;
import ddge.modules.exec.wrap_as_builder;

namespace ddge::exec {

inline namespace primitives {

export template <converts_to_task_builder_c... TaskBuilders_T>
    requires(std::same_as<typename as_task_builder_t<TaskBuilders_T>::Result, void> && ...)
[[nodiscard]]
auto in_parallel(TaskBuilders_T&&... task_builders) -> TaskBuilder<void>;

}   // namespace primitives

}   // namespace ddge::exec

template <ddge::exec::converts_to_task_builder_c... TaskBuilders_T>
    requires(
        std::same_as<typename ddge::exec::as_task_builder_t<TaskBuilders_T>::Result, void>
        && ...
    )
auto ddge::exec::primitives::in_parallel(TaskBuilders_T&&... task_builders)
    -> TaskBuilder<void>
{
    return TaskBuilder<void>{ [... wrapped_task_builders = wrap_as_builder(
                                   std::forward<TaskBuilders_T>(task_builders)
                               )](Nexus& nexus) -> Task<void> {
        return
            [... tasks = wrapped_task_builders.build(nexus)] -> void { (tasks(), ...); };
    } };
}
