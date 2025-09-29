module;

#include <concepts>
#include <memory>
#include <optional>
#include <utility>

export module ddge.modules.execution.v2.primitives.repeat;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.v2.Task;
import ddge.modules.execution.v2.TaskBuilder;
import ddge.modules.execution.v2.TaskFinishedCallback;
import ddge.modules.execution.v2.TaskHubBuilder;
import ddge.modules.execution.v2.TaskHubProxy;

namespace ddge::exec::v2 {

export template <std::integral Repetition_T>
auto repeat(
    TaskBuilder<void>&&         main_builder,
    TaskBuilder<Repetition_T>&& repetition_specifier_builder
) -> TaskBuilder<void>
{
    return TaskBuilder<void>{
        [x_main_builder                 = std::move(main_builder),
         x_repetition_specifier_builder = std::move(repetition_specifier_builder)](
            Nexus&                       nexus,
            TaskHubBuilder&              task_hub_builder,
            TaskFinishedCallback<void>&& callback
        ) mutable -> Task   //
        {
            class Looper {
            public:
                explicit Looper(TaskFinishedCallback<void>&& callback)
                    : m_callback{ std::move(callback) }
                {}

                auto set_repetition(Repetition_T repetition) -> void
                {
                    m_repetition = repetition;
                }
                auto set_looped_task(Task looped_task) -> void
                {
                    m_looped_task = looped_task;
                }
                auto schedule_loop_back(const TaskHubProxy& task_hub_proxy) -> void
                {
                    if (m_repetition > 0) {
                        --*m_repetition;
                        m_repetition->schedule(task_hub_proxy);
                    }
                    else {
                        m_callback(task_hub_proxy);
                    }
                }
                auto call_callback(const TaskHubProxy& task_hub_proxy) -> void
                {
                    m_callback(task_hub_proxy);
                }

            private:
                std::optional<Repetition_T> m_repetition;
                std::optional<Task>         m_looped_task;
                TaskFinishedCallback<void>  m_callback;
            };

            std::shared_ptr<Looper> looper{
                std::make_shared<Looper>(std::move(callback))   //
            };

            const Task main =
                std::move(x_main_builder)
                    .build(nexus, task_hub_builder, [looper](TaskHubProxy& task_hub_proxy) {
                        looper->schedule_loop_back(task_hub_proxy);
                    });
            looper->set_looped_task(main);

            Task repetition_task = std::move(x_repetition_specifier_builder)
                                       .build(
                                           nexus,   //
                                           task_hub_builder,
                                           [looper, main](
                                               const TaskHubProxy& task_hub_proxy,
                                               const Repetition_T  repetition
                                           ) -> void {
                                               if (repetition > 0) {
                                                   looper->set_repetition(repetition - 1);
                                                   main.schedule(task_hub_proxy);
                                               }
                                               else {
                                                   looper->call_callback(task_hub_proxy);
                                               }
                                           }
                                       );

            return repetition_task;
        }
    };
}

}   // namespace ddge::exec::v2
