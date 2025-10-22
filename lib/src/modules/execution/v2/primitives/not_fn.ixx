module;

#include <utility>

// TODO: remove once Clang can mangle
#include <function2/function2.hpp>

export module ddge.modules.execution.v2.primitives.not_fn;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.v2.Cardinality;
import ddge.modules.execution.v2.TaskBlueprint;
import ddge.modules.execution.v2.TaskBuilder;
import ddge.modules.execution.v2.TaskBundle;
import ddge.modules.execution.v2.TaskFinishedCallback;
import ddge.modules.execution.v2.TaskHubBuilder;
import ddge.modules.execution.v2.TaskHubProxy;

namespace ddge::exec::v2 {

export auto not_fn(TaskBlueprint<bool, Cardinality::eSingle>&& task_blueprint)
    -> TaskBlueprint<bool, Cardinality::eSingle>
{
    return TaskBlueprint<bool, Cardinality::eSingle>{
        [x_blueprint = std::move(task_blueprint)] mutable -> TaskBuilder<bool> {
            return TaskBuilder<bool>{
                [y_blueprint = std::move(x_blueprint)]   //
                (                                        //
                    Nexus & nexus,
                    TaskHubBuilder & task_hub_builder,
                    TaskFinishedCallback<bool>&& callback
                ) mutable -> TaskBundle   //
                {
                    return TaskBundle{
                        task_hub_builder.emplace(
                            [task = std::move(y_blueprint)
                                        .materialize()
                                        .build(
                                            nexus,
                                            task_hub_builder,
                                            [x_callback = std::move(callback)](
                                                const TaskHubProxy& task_hub_proxy,
                                                const bool          result
                                            ) mutable -> void {   //
                                                return x_callback(task_hub_proxy, !result);
                                            }
                                        )]   //
                            (const TaskHubProxy& task_hub_proxy) mutable -> void {
                                task(task_hub_proxy);
                            }
                        )   //
                    };
                }
            };
        }   //
    };
}

}   // namespace ddge::exec::v2
