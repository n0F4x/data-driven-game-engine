module;

#include <concepts>
#include <memory>
#include <optional>
#include <utility>

export module ddge.modules.execution.v2.primitives.repeat;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.v2.TaskBuilder;
import ddge.modules.execution.v2.TaskFinishedCallback;
import ddge.modules.execution.v2.TaskHubBuilder;
import ddge.modules.execution.v2.TaskHubProxy;
import ddge.modules.execution.v2.TaskIndex;

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
        ) mutable -> TaskIndex   //
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
                auto set_looped_task_index(TaskIndex looped_task_index) -> void
                {
                    m_looped_task_index = looped_task_index;
                }
                auto schedule_loop_back(const TaskHubProxy& task_hub_proxy) -> void
                {
                    if (m_repetition > 0) {
                        --*m_repetition;
                        task_hub_proxy.schedule(*m_looped_task_index);
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
                std::optional<TaskIndex>    m_looped_task_index;
                TaskFinishedCallback<void>  m_callback;
            };

            std::shared_ptr<Looper> looper{
                std::make_shared<Looper>(std::move(callback))   //
            };

            const TaskIndex main_task_index =   //
                std::move(x_main_builder)
                    .build(
                        nexus,   //
                        task_hub_builder,
                        [looper](const TaskHubProxy& task_hub_proxy) -> void {
                            looper->schedule_loop_back(task_hub_proxy);
                        }
                    );
            looper->set_looped_task_index(main_task_index);

            const TaskIndex repetition_specifier_task_index =   //
                std::move(x_repetition_specifier_builder)
                    .build(
                        nexus,   //
                        task_hub_builder,
                        [looper, main_task_index](
                            const TaskHubProxy& task_hub_proxy,
                            const Repetition_T  repetition
                        ) -> void {
                            if (repetition > 0) {
                                looper->set_repetition(repetition - 1);
                                task_hub_proxy.schedule(main_task_index);
                            }
                            else {
                                looper->call_callback(task_hub_proxy);
                            }
                        }
                    );

            return repetition_specifier_task_index;
        }
    };
}

}   // namespace ddge::exec::v2
