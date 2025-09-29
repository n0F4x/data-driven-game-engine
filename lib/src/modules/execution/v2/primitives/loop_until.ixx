module;

#include <memory>
#include <optional>
#include <utility>

export module ddge.modules.execution.v2.primitives.loop_until;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.v2.Task;
import ddge.modules.execution.v2.TaskBuilder;
import ddge.modules.execution.v2.TaskFinishedCallback;
import ddge.modules.execution.v2.TaskHubBuilder;
import ddge.modules.execution.v2.TaskHubProxy;

namespace ddge::exec::v2 {

export auto
    loop_until(TaskBuilder<void>&& main_builder, TaskBuilder<bool>&& predicate_builder)
        -> TaskBuilder<void>
{
    return TaskBuilder<void>{
        [x_main_builder      = std::move(main_builder),
         x_predicate_builder = std::move(predicate_builder)](
            Nexus&                       nexus,
            TaskHubBuilder&              task_hub_builder,
            TaskFinishedCallback<void>&& callback
        ) mutable -> Task   //
        {
            class Looper {
            public:
                auto set_task_predicate(Task predicate_task) -> void
                {
                    m_predicate_task = predicate_task;
                }
                auto schedule_loop_back(const TaskHubProxy& task_hub_proxy) const -> void
                {
                    m_predicate_task->schedule(task_hub_proxy);
                }

            private:
                std::optional<Task> m_predicate_task;
            };

            std::shared_ptr<Looper> looper{ std::make_shared<Looper>() };

            Task predicate_task =
                std::move(x_predicate_builder)
                    .build(
                        nexus,   //
                        task_hub_builder,
                        [x_callback = std::move(callback),
                         main       = std::move(x_main_builder)
                                    .build(
                                        nexus,
                                        task_hub_builder,
                                        [looper](const TaskHubProxy& task_hub_proxy) {
                                            looper->schedule_loop_back(task_hub_proxy);
                                        }
                                    )]                                //
                        (const TaskHubProxy& task_hub_proxy,
                         const bool          should_execute) mutable -> void   //
                        {
                            if (should_execute) {
                                main.schedule(task_hub_proxy);
                            }
                            else {
                                x_callback(task_hub_proxy);
                            }
                        }
                    );

            looper->set_task_predicate(predicate_task);

            return predicate_task;
        }
    };
}

}   // namespace ddge::exec::v2
