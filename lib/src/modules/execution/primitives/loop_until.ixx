module;

#include <concepts>
#include <utility>

export module ddge.modules.execution.primitives.loop_until;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.as_task_builder_t;
import ddge.modules.execution.converts_to_task_builder_c;
import ddge.modules.execution.Task;
import ddge.modules.execution.TaskBuilder;
import ddge.modules.execution.wrap_as_builder;

namespace ddge::exec {

inline namespace primitives {

export template <
    converts_to_task_builder_c TaskBuilder_T,
    converts_to_task_builder_c PredicateTaskBuilder_T>
    requires std::same_as<typename as_task_builder_t<TaskBuilder_T>::Result, void>
          && std::same_as<typename as_task_builder_t<PredicateTaskBuilder_T>::Result, bool>
[[nodiscard]]
auto loop_until(
    TaskBuilder_T&&          task_builder,
    PredicateTaskBuilder_T&& predicate_task_builder
) -> TaskBuilder<void>;

}   // namespace primitives

}   // namespace ddge::exec

template <
    ddge::exec::converts_to_task_builder_c TaskBuilder_T,
    ddge::exec::converts_to_task_builder_c PredicateTaskBuilder_T>
    requires std::same_as<
                 typename ddge::exec::as_task_builder_t<TaskBuilder_T>::Result,
                 void>
          && std::same_as<
                 typename ddge::exec::as_task_builder_t<PredicateTaskBuilder_T>::Result,
                 bool>
auto ddge::exec::loop_until(
    TaskBuilder_T&&          task_builder,
    PredicateTaskBuilder_T&& predicate_task_builder
) -> TaskBuilder<void>
{
    return TaskBuilder<void>{
        [wrapped_task_builder = wrap_as_builder(std::forward<TaskBuilder_T>(task_builder)),
         wrapped_predicate_task_builder = wrap_as_builder(
             std::forward<PredicateTaskBuilder_T>(predicate_task_builder)
         )](Nexus& nexus) -> Task<void> {
            return [task = wrapped_task_builder.build(nexus),
                    predicate_task =
                        wrapped_predicate_task_builder.build(nexus)] mutable -> void {
                while (predicate_task()) {
                    task();
                }
            };
        }
    };
}
