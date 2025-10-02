module;

#include <memory>
#include <optional>
#include <utility>

export module ddge.modules.execution.v2.primitives.loop_until;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.v2.TaskBuilder;
import ddge.modules.execution.v2.TaskFinishedCallback;
import ddge.modules.execution.v2.TaskHubBuilder;
import ddge.modules.execution.v2.TaskHubProxy;
import ddge.modules.execution.v2.TaskIndex;

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
        ) mutable -> TaskIndex   //
        {
            class Looper {
            public:
                auto set_task_predicate(TaskIndex predicate_task_index) -> void
                {
                    m_predicate_task_index = predicate_task_index;
                }
                auto schedule_loop_back(const TaskHubProxy& task_hub_proxy) const -> void
                {
                    task_hub_proxy.schedule(*m_predicate_task_index);
                }

            private:
                std::optional<TaskIndex> m_predicate_task_index;
            };

            std::shared_ptr<Looper> looper{ std::make_shared<Looper>() };

            const TaskIndex predicate_task_index =
                std::move(x_predicate_builder)
                    .build(
                        nexus,   //
                        task_hub_builder,
                        [x_callback = std::move(callback),
                         main_task_index =
                             std::move(x_main_builder)
                                 .build(
                                     nexus,
                                     task_hub_builder,
                                     [looper](const TaskHubProxy& task_hub_proxy) {
                                         looper->schedule_loop_back(task_hub_proxy);
                                     }
                                 )]                                   //
                        (const TaskHubProxy& task_hub_proxy,
                         const bool          should_execute) mutable -> void   //
                        {
                            if (should_execute) {
                                task_hub_proxy.schedule(main_task_index);
                            }
                            else {
                                x_callback(task_hub_proxy);
                            }
                        }
                    );

            looper->set_task_predicate(predicate_task_index);

            return predicate_task_index;
        }
    };
}

}   // namespace ddge::exec::v2
