module;

#include <algorithm>
#include <cassert>
#include <deque>
#include <expected>
#include <ranges>
#include <utility>
#include <vector>

module ddge.modules.scheduler.TaskHubBuilder;

import ddge.modules.scheduler.ErasedTaskFactory;
import ddge.modules.scheduler.Task;
import ddge.modules.scheduler.TaskHubProxy;

ddge::scheduler::TaskHubBuilder::TaskHubBuilder(Nexus& nexus) : m_nexus{ nexus } {}

auto ddge::scheduler::TaskHubBuilder::locks_of(const TaskIndex task_index) const
    -> const LockGroup&
{
    return get(task_index).locks();
}

auto ddge::scheduler::TaskHubBuilder::build(
    const uint32_t number_of_threads
) && -> std::unique_ptr<TaskHub>
{
    std::unique_ptr<TaskHub> result{
        std::make_unique<TaskHub>(
            m_generic_task_factories.size(),
            m_main_only_task_factories.size(),
            number_of_threads
        )   //
    };


    // indirect tasks must be built first as they change the builders for embedded ones
    // indirect tasks should also be built in reverse order as they modify previous task
    // factories
    std::vector<Task> indirect_tasks{
        std::from_range,
        std::views::as_rvalue(std::move(m_indirect_task_factories))   //
            | std::views::reverse
            | std::views::transform(
                std::bind_back(&ErasedTaskFactory::build, TaskHubProxy{ *result })
            )
    };
    std::ranges::reverse(indirect_tasks);
    result->set_indirect_tasks(std::move(indirect_tasks));

    for (auto&& [i, task_factory] : std::views::zip(
             std::views::iota(0u, m_generic_task_factories.size()),
             std::views::as_rvalue(std::move(m_generic_task_factories))
         ))
    {
        const std::expected expected = result->try_emplace_generic_at(
            std::move(task_factory).build(TaskHubProxy{ *result }),
            TaskIndex{ i | TaskHub::IndexTagMasks::generic }
        );
        assert(expected.has_value());
    }

    for (auto&& [i, task_factory] : std::views::zip(
             std::views::iota(0u, m_main_only_task_factories.size()),
             std::views::as_rvalue(std::move(m_main_only_task_factories))
         ))
    {
        const std::expected expected = result->try_emplace_main_only_at(
            std::move(task_factory).build(TaskHubProxy{ *result }),
            TaskIndex{ i | TaskHub::IndexTagMasks::main_only }
        );
        assert(expected.has_value());
    }


    return result;
}
