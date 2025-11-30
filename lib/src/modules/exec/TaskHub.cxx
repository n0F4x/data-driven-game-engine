module;

#include <expected>
#include <format>
#include <utility>

#include "utility/contract_macros.hpp"

module ddge.modules.exec.TaskHub;

import ddge.modules.exec.scheduler.Work;
import ddge.modules.exec.scheduler.WorkContinuation;
import ddge.modules.exec.scheduler.WorkIndex;

import ddge.utility.any_cast;
import ddge.utility.contracts;

ddge::exec::TaskHub::TaskHub(
    const uint64_t generic_task_capacity,
    const uint64_t main_only_task_capacity,
    const uint32_t number_of_threads
)
    : m_generic_work_tree{ generic_task_capacity, number_of_threads },
      m_main_only_work_tree{ main_only_task_capacity, 1 }
{}

auto ddge::exec::TaskHub::set_indirect_tasks(std::vector<Task>&& indirect_tasks) -> void
{
    m_indirect_tasks = std::move(indirect_tasks);
}

auto ddge::exec::TaskHub::try_emplace_generic_at(Task&& task, const TaskIndex task_index)
    -> std::expected<void, Task>
{
    PRECOND((task_index.underlying() & task_index_tag_mask) == IndexTagMasks::generic);

    return try_emplace_embedded_task_at(std::move(task), task_index);
}

auto ddge::exec::TaskHub::try_emplace_main_only_at(Task&& task, const TaskIndex task_index)
    -> std::expected<void, Task>
{
    PRECOND((task_index.underlying() & task_index_tag_mask) == IndexTagMasks::main_only);

    return try_emplace_embedded_task_at(std::move(task), task_index);
}

#define STRINGIFY(x) #x

auto ddge::exec::TaskHub::schedule(const TaskIndex task_index) -> void
{
    const TaskIndex::Underlying task_index_value{
        task_index.underlying() & task_index_value_mask   //
    };

    if (const TaskIndex::Underlying type{ task_index.underlying() & task_index_tag_mask };
        type == IndexTagMasks::generic)
    {
        m_generic_work_tree.schedule(WorkIndex{ task_index_value });
    }
    else if (type == IndexTagMasks::main_only) {
        m_main_only_work_tree.schedule(WorkIndex{ task_index_value });
    }
    else if (type == IndexTagMasks::indirect) {
        m_indirect_tasks[task_index_value]();
    }
    else {
        PRECOND(
            false,
            std::format("invalid {} ({:b})", STRINGIFY(task_index), task_index.underlying())
        );
        std::unreachable();
    }
}

auto ddge::exec::TaskHub::try_execute_a_generic_task(const uint32_t thread_id) -> bool
{
    return m_generic_work_tree.try_execute_one_work(thread_id);
}

auto ddge::exec::TaskHub::try_execute_a_main_only_task() -> bool
{
    return m_main_only_work_tree.try_execute_one_work(0);
}

auto ddge::exec::TaskHub::try_emplace_embedded_task_at(
    Task&&          task,
    const TaskIndex task_index
) -> std::expected<void, Task>
{
    WorkTree& work_tree{ select_work_tree(task_index) };

    struct WorkTask {
        Task task;

        auto operator()() -> WorkContinuation
        {
            task();
            return WorkContinuation::eDontCare;
        }
    };

    return work_tree
        .try_emplace_at(
            WorkIndex{ task_index.underlying() & task_index_value_mask },
            Work{ WorkTask{ .task = std::move(task) } }
        )
        .transform_error([](Work&& work) -> Task {
            return util::any_cast<WorkTask>(std::move(work)).task;
        });
}

auto ddge::exec::TaskHub::select_work_tree(const TaskIndex task_index) -> WorkTree&
{
    if ((task_index.underlying() & task_index_tag_mask) == IndexTagMasks::generic) {
        return m_generic_work_tree;
    }
    if ((task_index.underlying() & task_index_tag_mask) == IndexTagMasks::main_only) {
        return m_main_only_work_tree;
    }
    std::unreachable();
}
