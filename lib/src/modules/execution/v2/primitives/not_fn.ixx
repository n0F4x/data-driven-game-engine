module;

#include <utility>

export module ddge.modules.execution.v2.primitives.not_fn;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.v2.TaskBuilder;
import ddge.modules.execution.v2.TaskFinishedCallback;
import ddge.modules.execution.v2.TaskHubBuilder;
import ddge.modules.execution.v2.TaskHubProxy;
import ddge.modules.execution.v2.TaskIndex;

namespace ddge::exec::v2 {

export auto not_fn(TaskBuilder<bool>&& task_builder) -> TaskBuilder<bool>
{
    return TaskBuilder<bool>{
        [x_builder = std::move(task_builder)]   //
        (                                       //
            Nexus & nexus,
            TaskHubBuilder & task_hub_builder,
            TaskFinishedCallback<bool>&& callback
        ) mutable -> TaskIndex   //
        {
            return task_hub_builder.emplace(
                [task_index = std::move(x_builder).build(
                     nexus,
                     task_hub_builder,
                     [x_callback = std::move(callback)](
                         const TaskHubProxy& task_hub_proxy, const bool result
                     ) mutable -> void {   //
                         return x_callback(task_hub_proxy, !result);
                     }
                 )](        //
                    const TaskHubProxy& task_hub_proxy
                ) -> void   //
                {           //
                    task_hub_proxy.schedule(task_index);
                }
            );
        }
    };
}

}   // namespace ddge::exec::v2
