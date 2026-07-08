module;

#include <utility>

export module ddge.scheduler.primitives.loop_until;

import ddge.scheduler.locks.LockGroup;
import ddge.scheduler.IndirectTaskContinuationSetter;
import ddge.scheduler.IndirectTaskFactory;
import ddge.scheduler.TaskBuilder;
import ddge.scheduler.TaskContinuation;
import ddge.scheduler.TaskContinuationFactory;
import ddge.scheduler.TaskHubBuilder;
import ddge.scheduler.TaskHubProxy;
import ddge.scheduler.TypedTaskFactoryHandle;

import ddge.util.no_op;

namespace ddge::scheduler {

export [[nodiscard]]
auto loop_until(TaskBuilder<void>&& body_builder, TaskBuilder<bool>&& predicate_builder)
    -> TaskBuilder<void>;

}   // namespace ddge::scheduler

auto ddge::scheduler::loop_until(
    TaskBuilder<void>&& body_builder,
    TaskBuilder<bool>&& predicate_builder
) -> TaskBuilder<void>
{
    return TaskBuilder<void>{
        [x_body_builder      = std::move(body_builder),
         x_predicate_builder = std::move(predicate_builder)](
            TaskHubBuilder& task_hub_builder
        ) mutable -> TypedTaskFactoryHandle<void>   //
        {
            const TypedTaskFactoryHandle<void> body_task_handle =
                std::move(x_body_builder).build(task_hub_builder);

            const TypedTaskFactoryHandle<bool> predicate_task_handle =
                std::move(x_predicate_builder).build(task_hub_builder);

            body_task_handle->set_continuation_factory(
                TaskContinuationFactory<void>{
                    [predicate_task_handle](const TaskHubProxy task_hub_proxy)
                        -> TaskContinuation<void> {
                        return TaskContinuation<void>{
                            [predicate_task_index = predicate_task_handle.index(),
                             task_hub_proxy] -> void {
                                task_hub_proxy.schedule(predicate_task_index);
                            }   //
                        };
                    }   //
                }
            );

            auto set_continuation = [body_task_index = body_task_handle.index(),
                                     predicate_task_handle](
                                        TaskContinuation<void>&& continuation
                                    ) -> void   //
            {
                predicate_task_handle->set_continuation_factory(
                    TaskContinuationFactory<bool>{
                        [body_task_index,
                         x_continuation = std::move(continuation)](
                            const TaskHubProxy task_hub_proxy
                        ) mutable -> TaskContinuation<bool>   //
                        {
                            return TaskContinuation<bool>{
                                [body_task_index,
                                 task_hub_proxy,
                                 y_continuation = std::move(x_continuation)](
                                    const bool predicate
                                ) mutable -> void {
                                    if (predicate) {
                                        task_hub_proxy.schedule(body_task_index);
                                    }
                                    else {
                                        y_continuation();
                                    }
                                }   //
                            };
                        }   //
                    }
                );
            };

            set_continuation(TaskContinuation<void>{ util::no_op });

            LockGroup locks;
            locks.expand(body_task_handle->locks());
            locks.expand(predicate_task_handle->locks());

            return task_hub_builder.emplace_indirect_task_factory(
                IndirectTaskFactory<void>{
                    predicate_task_handle.index(),
                    IndirectTaskContinuationSetter<void>{
                        std::move(set_continuation)   //
                    },
                    std::move(locks)                  //
                }
            );
        }   //
    };
}
