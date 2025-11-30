module;

#include <memory>
#include <optional>
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
import ddge.modules.scheduler.TypedTaskIndex;

namespace ddge::scheduler {

export [[nodiscard]]
auto not_fn(TaskBuilder<bool>&& task_builder) -> TaskBuilder<bool>;

}   // namespace ddge::scheduler

auto ddge::scheduler::not_fn(TaskBuilder<bool>&& task_builder) -> TaskBuilder<bool>
{
    return TaskBuilder<bool>{
        [x_task_builder = std::move(task_builder)](
            TaskHubBuilder& task_hub_builder
        ) mutable -> TypedTaskIndex<bool>   //
        {
            const TypedTaskIndex<bool> inner_task_index =
                std::move(x_task_builder).build(task_hub_builder);

            std::shared_ptr<std::optional<TaskContinuation<bool>>> shared_continuation{
                std::make_shared<std::optional<TaskContinuation<bool>>>()
            };

            task_hub_builder.set_task_continuation_factory(
                inner_task_index,
                TaskContinuationFactory<bool>{
                    [shared_continuation](const TaskHubProxy) mutable
                        -> TaskContinuation<bool> {
                        return TaskContinuation<bool>{
                            [shared_continuation](const bool result) mutable -> void {   //
                                if (shared_continuation->has_value()) {
                                    (**shared_continuation)(!result);
                                }
                            }   //
                        };
                    }   //
                }
            );

            return task_hub_builder.emplace_indirect_task_factory(
                IndirectTaskFactory<bool>{
                    inner_task_index,
                    IndirectTaskContinuationSetter<bool>{
                        [x_shared_continuation = std::move(shared_continuation)](
                            TaskContinuation<bool>&& continuation
                        ) mutable -> void {
                            *x_shared_continuation = std::move(continuation);
                        }   //
                    },
                    LockGroup{ task_hub_builder.locks_of(inner_task_index) } }
            );
        }
    };
}
