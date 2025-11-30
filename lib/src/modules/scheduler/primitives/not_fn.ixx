module;

#include <memory>
#include <optional>
#include <utility>

export module ddge.modules.scheduler.primitives.not_fn;

import ddge.modules.scheduler.locks.LockGroup;
import ddge.modules.scheduler.Cardinality;
import ddge.modules.scheduler.IndirectTaskContinuationSetter;
import ddge.modules.scheduler.IndirectTaskFactory;
import ddge.modules.scheduler.TaskBlueprint;
import ddge.modules.scheduler.TaskBuilder;
import ddge.modules.scheduler.TaskContinuation;
import ddge.modules.scheduler.TaskContinuationFactory;
import ddge.modules.scheduler.TaskHubBuilder;
import ddge.modules.scheduler.TaskHubProxy;
import ddge.modules.scheduler.TypedTaskIndex;

namespace ddge::scheduler {

export auto not_fn(TaskBlueprint<bool, Cardinality::eSingle>&& task_blueprint)
    -> TaskBlueprint<bool, Cardinality::eSingle>
{
    return TaskBlueprint<bool, Cardinality::eSingle>{
        [x_blueprint = std::move(task_blueprint)] mutable -> TaskBuilder<bool> {
            return TaskBuilder<bool>{
                [y_blueprint = std::move(x_blueprint)]   //
                (                                        //
                    TaskHubBuilder & task_hub_builder
                ) mutable -> TypedTaskIndex<bool>        //
                {
                    const TypedTaskIndex<bool> inner_task_index =
                        std::move(y_blueprint).materialize().build(task_hub_builder);

                    std::shared_ptr<std::optional<TaskContinuation<bool>>>
                        shared_continuation{
                            std::make_shared<std::optional<TaskContinuation<bool>>>()
                        };

                    task_hub_builder.set_task_continuation_factory(
                        inner_task_index,
                        TaskContinuationFactory<bool>{
                            [shared_continuation](const TaskHubProxy) mutable
                                -> TaskContinuation<bool> {
                                return TaskContinuation<bool>{
                                    [shared_continuation](
                                        const bool result
                                    ) mutable -> void {   //
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
        }   //
    };
}

}   // namespace ddge::scheduler
