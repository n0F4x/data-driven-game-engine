module;

#include <memory>
#include <utility>

export module ddge.modules.execution.v2.primitives.run_if;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.v2.TaskBuilder;
import ddge.modules.execution.v2.TaskFinishedCallback;
import ddge.modules.execution.v2.TaskHubBuilder;
import ddge.modules.execution.v2.TaskHubProxy;
import ddge.modules.execution.v2.TaskIndex;

namespace ddge::exec::v2 {

export auto run_if(TaskBuilder<void>&& main_builder, TaskBuilder<bool>&& predicate_builder)
    -> TaskBuilder<void>
{
    return TaskBuilder<void>{
        [x_main_builder      = std::move(main_builder),
         x_predicate_builder = std::move(predicate_builder)](
            Nexus&                       nexus,
            TaskHubBuilder&              task_hub_builder,
            TaskFinishedCallback<void>&& callback
        ) mutable -> TaskIndex   //
        {
            const std::shared_ptr<TaskFinishedCallback<void>> shared_callback{
                std::make_shared<TaskFinishedCallback<void>>(std::move(callback))
            };

            return std::move(x_predicate_builder)
                .build(
                    nexus,   //
                    task_hub_builder,
                    [main_task_index =
                         std::move(x_main_builder)
                             .build(
                                 nexus,
                                 task_hub_builder,
                                 [shared_callback](const TaskHubProxy& task_hub_proxy) {
                                     shared_callback->operator()(task_hub_proxy);
                                 }
                             ),
                     shared_callback]                     //
                    (const TaskHubProxy& task_hub_proxy,
                     const bool          should_execute) -> void   //
                    {
                        if (should_execute) {
                            task_hub_proxy.schedule(main_task_index);
                        }
                        else {
                            shared_callback->operator()(task_hub_proxy);
                        }
                    }
                );
        }
    };
}

}   // namespace ddge::exec::v2
