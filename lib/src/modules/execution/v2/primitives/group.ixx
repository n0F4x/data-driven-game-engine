module;

#include <atomic>
#include <concepts>
#include <memory>
#include <utility>

export module ddge.modules.execution.v2.primitives.group;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.v2.Task;
import ddge.modules.execution.v2.TaskBuilder;
import ddge.modules.execution.v2.TaskFinishedCallback;
import ddge.modules.execution.v2.TaskHubBuilder;
import ddge.modules.execution.v2.TaskHubProxy;

namespace ddge::exec::v2 {

export template <std::same_as<void>... Voids_T>
auto group(TaskBuilder<Voids_T>... builders) -> TaskBuilder<void>
{
    return TaskBuilder<void>{
        [... x_builders = std::move(builders)]   //
        (                                        //
            Nexus & nexus,
            TaskHubBuilder & task_hub_builder,
            TaskFinishedCallback<void>&& callback
        ) mutable -> Task   //
        {
            class Consumer {
            public:
                explicit Consumer(
                    const uint32_t               producer_count,
                    TaskFinishedCallback<void>&& callback
                )
                    : m_number_of_producers{ producer_count },
                      m_counter{ producer_count },
                      m_callback{ std::move(callback) }
                {}

                auto arrive(const TaskHubProxy& task_hub_proxy) -> void
                {
                    if (m_counter.fetch_sub(1) - 1 == 0) {
                        m_counter.store(m_number_of_producers);
                        m_callback(task_hub_proxy);
                    }
                }

            private:
                uint32_t                   m_number_of_producers;
                std::atomic_uint32_t       m_counter;
                std::atomic_bool           m_result{ true };
                TaskFinishedCallback<void> m_callback;
            };

            const std::shared_ptr<Consumer> consumer{
                std::make_shared<Consumer>(sizeof...(Voids_T), std::move(callback))
            };

            return Task{
                task_hub_builder.emplace(
                    [... tasks = std::move(x_builders)
                                     .build(
                                         nexus,
                                         task_hub_builder,
                                         [consumer](const TaskHubProxy& task_hub_proxy) {
                                             consumer->arrive(task_hub_proxy);
                                         }
                                     )]                    //
                    (const TaskHubProxy& task_hub_proxy)   //
                    {
                        // TODO: account for shared resources
                        (tasks.schedule(task_hub_proxy), ...);
                    }
                )   //
            };
        }
    };
}

}   // namespace ddge::exec::v2
