module;

#include <concepts>
#include <utility>

export module ddge.modules.scheduler.primitives.run_if;

import ddge.modules.scheduler.Nexus;
import ddge.modules.scheduler.as_task_builder_t;
import ddge.modules.scheduler.converts_to_task_builder_c;
import ddge.modules.scheduler.Task;
import ddge.modules.scheduler.TaskBuilder;
import ddge.modules.scheduler.wrap_as_builder;

namespace ddge::scheduler {

inline namespace primitives {

export template <
    converts_to_task_builder_c TaskBuilder_T,
    converts_to_task_builder_c PredicateTaskBuilder_T>
    requires std::same_as<typename as_task_builder_t<TaskBuilder_T>::Result, void>
          && std::same_as<typename as_task_builder_t<PredicateTaskBuilder_T>::Result, bool>
[[nodiscard]]
auto run_if(TaskBuilder_T&& task_builder, PredicateTaskBuilder_T&& predicate_task_builder)
    -> TaskBuilder<void>;

}   // namespace primitives

}   // namespace ddge::scheduler

template <
    ddge::scheduler::converts_to_task_builder_c TaskBuilder_T,
    ddge::scheduler::converts_to_task_builder_c PredicateTaskBuilder_T>
    requires std::same_as<
                 typename ddge::scheduler::as_task_builder_t<TaskBuilder_T>::Result,
                 void>
          && std::same_as<
                 typename ddge::scheduler::as_task_builder_t<PredicateTaskBuilder_T>::Result,
                 bool>
auto ddge::scheduler::run_if(
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
                if (predicate_task()) {
                    task();
                }
            };
        }
    };
}
