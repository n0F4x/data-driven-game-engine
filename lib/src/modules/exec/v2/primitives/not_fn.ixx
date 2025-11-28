module;

#include <memory>
#include <optional>
#include <utility>

export module ddge.modules.exec.v2.primitives.not_fn;

import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.IndirectTaskContinuationSetter;
import ddge.modules.exec.v2.IndirectTaskFactory;
import ddge.modules.exec.v2.TaskBlueprint;
import ddge.modules.exec.v2.TaskBuilder;
import ddge.modules.exec.v2.TaskContinuation;
import ddge.modules.exec.v2.TaskContinuationFactory;
import ddge.modules.exec.v2.TaskHubBuilder;
import ddge.modules.exec.v2.TaskHubProxy;
import ddge.modules.exec.v2.TypedTaskIndex;

namespace ddge::exec::v2 {

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
                            }   //
                        }
                    );
                }
            };
        }   //
    };
}

}   // namespace ddge::exec::v2
