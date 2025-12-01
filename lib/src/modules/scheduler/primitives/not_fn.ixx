module;

#include <utility>

export module ddge.modules.scheduler.primitives.not_fn;

import ddge.modules.scheduler.locks.LockGroup;
import ddge.modules.scheduler.IndirectTaskContinuationSetter;
import ddge.modules.scheduler.IndirectTaskFactory;
import ddge.modules.scheduler.TaskBuilder;
import ddge.modules.scheduler.TaskContinuation;
import ddge.modules.scheduler.TaskContinuationFactory;
import ddge.modules.scheduler.TaskHubBuilder;
import ddge.modules.scheduler.TaskHubProxy;
import ddge.modules.scheduler.TypedTaskFactoryHandle;

import ddge.utility.no_op;

namespace ddge::scheduler {

export [[nodiscard]]
auto not_fn(TaskBuilder<bool>&& task_builder) -> TaskBuilder<bool>;

}   // namespace ddge::scheduler

auto ddge::scheduler::not_fn(TaskBuilder<bool>&& task_builder) -> TaskBuilder<bool>
{
    return TaskBuilder<bool>{
        [x_task_builder = std::move(task_builder)](
            TaskHubBuilder& task_hub_builder
        ) mutable -> TypedTaskFactoryHandle<bool>   //
        {
            const TypedTaskFactoryHandle<bool> inner_task_handle =
                std::move(x_task_builder).build(task_hub_builder);

            auto set_continuation =
                [inner_task_handle](TaskContinuation<bool>&& continuation) -> void   //
            {
                inner_task_handle->set_continuation_factory(
                    TaskContinuationFactory<bool>{
                        [x_continuation = std::move(continuation)](
                            const TaskHubProxy
                        ) mutable -> TaskContinuation<bool>   //
                        {
                            return TaskContinuation<bool>{
                                [y_continuation = std::move(x_continuation)](
                                    const bool result
                                ) mutable -> void { y_continuation(!result); }   //
                            };
                        }   //
                    }
                );
            };

            return task_hub_builder.emplace_indirect_task_factory(
                IndirectTaskFactory<bool>{
                    inner_task_handle.index(),
                    IndirectTaskContinuationSetter<bool>{ std::move(set_continuation) },
                    LockGroup{ inner_task_handle->locks() }   //
                }
            );
        }
    };
}
