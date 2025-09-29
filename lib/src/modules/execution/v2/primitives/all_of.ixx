module;

#include <atomic>
#include <concepts>
#include <memory>
#include <utility>

export module ddge.modules.execution.v2.primitives.all_of;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.v2.Task;
import ddge.modules.execution.v2.TaskBuilder;
import ddge.modules.execution.v2.TaskFinishedCallback;
import ddge.modules.execution.v2.TaskHubBuilder;
import ddge.modules.execution.v2.TaskHubProxy;

namespace ddge::exec::v2 {

export template <std::same_as<bool>... Bools_T>
auto all_of(TaskBuilder<Bools_T>&&... builders) -> TaskBuilder<bool>
{
    return TaskBuilder<bool>{
        [... x_builders = std::move(builders)](
            Nexus&                            nexus,
            TaskHubBuilder&                   task_hub_builder,
            const TaskFinishedCallback<bool>& callback
        ) mutable -> Task   //
        {
            class Consumer {
            public:
                explicit Consumer(
                    const uint32_t               producer_count,
                    TaskFinishedCallback<bool>&& callback
                )
                    : m_number_of_producers{ producer_count },
                      m_counter{ m_number_of_producers },
                      m_callback{ std::move(callback) }
                {}

                auto arrive(const TaskHubProxy& task_hub_proxy, const bool value) -> void
                {
                    if (value == false) {
                        m_result = false;
                    }

                    if (m_counter.fetch_sub(1) - 1 == 0) {
                        m_counter.store(m_number_of_producers);
                        m_callback(task_hub_proxy, m_result.exchange(true));
                    }
                }

            private:
                uint32_t                   m_number_of_producers;
                std::atomic_uint32_t       m_counter;
                std::atomic_bool           m_result{ true };
                TaskFinishedCallback<bool> m_callback;
            };

            auto consumer = std::make_shared<Consumer>(sizeof...(Bools_T), callback);
            return Task{
                [... tasks = std::move(x_builders)
                                 .build(
                                     nexus,
                                     task_hub_builder,
                                     [consumer](
                                         TaskHubProxy& task_hub_proxy, const bool result
                                     ) { consumer->arrive(task_hub_proxy, result); }
                                 )] {
                    // TODO: account for shared resources
                    (tasks.schedule(), ...);
                }
            };
        }
    };
}

}   // namespace ddge::exec::v2
