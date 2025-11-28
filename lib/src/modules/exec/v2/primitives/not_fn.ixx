module;

#include <memory>
#include <optional>
#include <utility>

export module ddge.modules.exec.v2.primitives.not_fn;

import ddge.modules.exec.v2.Cardinality;
import ddge.modules.exec.v2.IndirectTaskBody;
import ddge.modules.exec.v2.IndirectTaskContinuationSetter;
import ddge.modules.exec.v2.IndirectTaskFactory;
import ddge.modules.exec.v2.TaskBlueprint;
import ddge.modules.exec.v2.TaskBuilder;
import ddge.modules.exec.v2.TaskContinuation;
import ddge.modules.exec.v2.TaskFinishedCallback;
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
                    TaskHubBuilder & task_hub_builder,
                    TaskFinishedCallback<bool>&& callback
                ) mutable -> TypedTaskIndex<bool>   //
                {
                    std::shared_ptr<std::optional<TaskContinuation<bool>>>
                        shared_continuation{
                            std::make_shared<std::optional<TaskContinuation<bool>>>()
                        };

                    return task_hub_builder.emplace_indirect_task_factory(
                        IndirectTaskFactory<bool>{
                            IndirectTaskBody{
                                [task_index =
                                     std::move(y_blueprint)
                                         .materialize()
                                         .build(
                                             task_hub_builder,
                                             TaskFinishedCallback<bool>{
                                                 [x_callback = std::move(callback),
                                                  shared_continuation](
                                                     const TaskHubProxy& task_hub_proxy,
                                                     const bool          result
                                                 ) mutable -> void {   //
                                                     if (shared_continuation->has_value()) {
                                                         (**shared_continuation)(!result);
                                                     }
                                                     else {
                                                         x_callback(
                                                             task_hub_proxy, !result
                                                         );
                                                     }
                                                 }   //
                                             }
                                         )]   //
                                (const TaskHubProxy& task_hub_proxy) mutable -> void {
                                    task_hub_proxy.schedule(task_index);
                                }   //
                            },
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
