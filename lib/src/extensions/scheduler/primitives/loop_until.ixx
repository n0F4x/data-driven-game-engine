module;

#include <concepts>
#include <utility>

export module extensions.scheduler.primitives.loop_until;

import core.scheduler.Nexus;
import core.scheduler.as_task_builder_t;
import core.scheduler.converts_to_task_builder_c;
import core.scheduler.Task;
import core.scheduler.TaskBuilder;
import core.scheduler.wrap_as_builder;

namespace extensions::scheduler {

inline namespace primitives {

export template <
    core::scheduler::converts_to_task_builder_c TaskBuilder_T,
    core::scheduler::converts_to_task_builder_c PredicateTaskBuilder_T>
    requires std::same_as<
                 typename core::scheduler::as_task_builder_t<TaskBuilder_T>::Result,
                 void>
          && std::same_as<
                 typename core::scheduler::as_task_builder_t<PredicateTaskBuilder_T>::Result,
                 bool>
[[nodiscard]]
auto loop_until(
    TaskBuilder_T&&          task_builder,
    PredicateTaskBuilder_T&& predicate_task_builder
) -> core::scheduler::TaskBuilder<void>;

}   // namespace primitives

}   // namespace extensions::scheduler

template <
    core::scheduler::converts_to_task_builder_c TaskBuilder_T,
    core::scheduler::converts_to_task_builder_c PredicateTaskBuilder_T>
    requires std::same_as<
                 typename core::scheduler::as_task_builder_t<TaskBuilder_T>::Result,
                 void>
          && std::same_as<
                 typename core::scheduler::as_task_builder_t<PredicateTaskBuilder_T>::Result,
                 bool>
auto extensions::scheduler::loop_until(
    TaskBuilder_T&&          task_builder,
    PredicateTaskBuilder_T&& predicate_task_builder
) -> core::scheduler::TaskBuilder<void>
{
    return core::scheduler::TaskBuilder<void>{
        [wrapped_task_builder =
             core::scheduler::wrap_as_builder(std::forward<TaskBuilder_T>(task_builder)),
         wrapped_predicate_task_builder = core::scheduler::wrap_as_builder(
             std::forward<PredicateTaskBuilder_T>(predicate_task_builder)
         )](core::scheduler::Nexus& nexus) -> core::scheduler::Task<void> {
            return
                [task           = wrapped_task_builder.build(nexus),
                 predicate_task = wrapped_predicate_task_builder.build(nexus)] -> void {
                    while (predicate_task()) {
                        task();
                    }
                };
        }
    };
}
