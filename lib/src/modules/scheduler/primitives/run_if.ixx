module;

#include <memory>
#include <optional>
#include <utility>

export module ddge.modules.scheduler.primitives.run_if;

import ddge.modules.scheduler.locks.LockGroup;
import ddge.modules.scheduler.IndirectTaskContinuationSetter;
import ddge.modules.scheduler.IndirectTaskFactory;
import ddge.modules.scheduler.TaskBuilder;
import ddge.modules.scheduler.TaskContinuation;
import ddge.modules.scheduler.TaskContinuationFactory;
import ddge.modules.scheduler.TaskHubBuilder;
import ddge.modules.scheduler.TaskHubProxy;
import ddge.modules.scheduler.TypedTaskIndex;

namespace ddge::scheduler {

export [[nodiscard]]
auto run_if(TaskBuilder<void>&& body_builder, TaskBuilder<bool>&& predicate_builder)
    -> TaskBuilder<void>;

}   // namespace ddge::scheduler

auto ddge::scheduler::run_if(
    TaskBuilder<void>&& body_builder,
    TaskBuilder<bool>&& predicate_builder
) -> TaskBuilder<void>
{
    return TaskBuilder<void>{
        [x_body_builder      = std::move(body_builder),
         x_predicate_builder = std::move(predicate_builder)](
            TaskHubBuilder& task_hub_builder
        ) mutable -> TypedTaskIndex<void>   //
        {
            const TypedTaskIndex<void> body_task_index =
                std::move(x_body_builder).build(task_hub_builder);

            std::shared_ptr<std::optional<TaskContinuation<void>>> shared_continuation{
                std::make_shared<std::optional<TaskContinuation<void>>>()
            };

            task_hub_builder.set_task_continuation_factory(
                body_task_index,
                TaskContinuationFactory<void>{
                    [shared_continuation](const TaskHubProxy) mutable
                        -> TaskContinuation<void> {
                        return TaskContinuation<void>{
                            [shared_continuation] mutable -> void {
                                if (shared_continuation->has_value()) {
                                    (**shared_continuation)();
                                }
                            }   //
                        };
                    }   //
                }
            );

            const TypedTaskIndex<bool> predicate_task_index =
                std::move(x_predicate_builder).build(task_hub_builder);

            task_hub_builder.set_task_continuation_factory(
                predicate_task_index,
                TaskContinuationFactory<bool>{
                    [body_task_index, shared_continuation](
                        const TaskHubProxy task_hub_proxy
                    ) mutable -> TaskContinuation<bool> {
                        return TaskContinuation<bool>{
                            [body_task_index,
                             shared_continuation,
                             task_hub_proxy](const bool should_execute) mutable -> void {
                                if (should_execute) {
                                    task_hub_proxy.schedule(body_task_index);
                                }
                                else {
                                    if (shared_continuation->has_value()) {
                                        (**shared_continuation)();
                                    }
                                }
                            }   //
                        };
                    }   //
                }
            );

            LockGroup locks;
            locks.expand(task_hub_builder.locks_of(body_task_index));
            locks.expand(task_hub_builder.locks_of(predicate_task_index));

            return task_hub_builder.emplace_indirect_task_factory(
                IndirectTaskFactory<void>{
                    predicate_task_index,
                    IndirectTaskContinuationSetter<void>{
                        [x_shared_continuation = std::move(shared_continuation)](
                            TaskContinuation<void>&& continuation
                        ) mutable -> void {
                            *x_shared_continuation = std::move(continuation);
                        }   //
                    },
                    std::move(locks) }
            );
        }   //
    };
}
