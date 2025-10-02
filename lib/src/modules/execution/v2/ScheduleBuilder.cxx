module;

#include <utility>

module ddge.modules.execution.v2.ScheduleBuilder;

import ddge.modules.execution.Nexus;
import ddge.modules.execution.v2.TaskFinishedCallback;
import ddge.modules.execution.v2.TaskHubBuilder;
import ddge.modules.execution.v2.TaskHubProxy;
import ddge.modules.execution.v2.TaskIndex;

ddge::exec::v2::ScheduleBuilder::ScheduleBuilder(TaskBuilder<void>&& initial)
    : m_task_builder{ std::move(initial) }
{}

auto ddge::exec::v2::ScheduleBuilder::then(TaskBuilder<void>&& next) && -> ScheduleBuilder
{
    TaskBuilder<void> previous{ std::move(m_task_builder) };

    m_task_builder = TaskBuilder<void>{
        [x_previous = std::move(previous), x_next = std::move(next)]   //
        (Nexus & nexus,
         TaskHubBuilder & task_hub_builder,
         TaskFinishedCallback<void>&& callback) mutable -> TaskIndex   //
        {
            return std::move(x_previous)
                .build(
                    nexus,
                    task_hub_builder,
                    [next_task_index = std::move(x_next).build(
                         nexus, task_hub_builder, std::move(callback)
                     )]                                            //
                    (const TaskHubProxy& task_hub_proxy) -> void   //
                    {                                              //
                        task_hub_proxy.schedule(next_task_index);
                    }
                );
        }   //
    };

    return std::move(*this);
}

ddge::exec::v2::ScheduleBuilder::operator TaskBuilder<void>() &&
{
    return std::move(m_task_builder);
}
