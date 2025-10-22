module;

#include <expected>
#include <format>
#include <utility>

#include "utility/contracts_macros.hpp"

module ddge.modules.exec.v2.TaskHub;

import ddge.modules.exec.scheduler.WorkContinuation;
import ddge.modules.exec.scheduler.WorkIndex;

import ddge.utility.contracts;

ddge::exec::v2::TaskHub::TaskHub(
    const uint64_t generic_capacity,
    const uint64_t main_only_capacity,
    const uint32_t number_of_threads
)
    : m_generic_work_tree{ generic_capacity, number_of_threads },
      m_main_only_work_tree{ main_only_capacity, 1 }
{}

auto ddge::exec::v2::TaskHub::try_emplace_generic_at(
    Task&&          task,
    const TaskIndex task_index
) -> std::expected<void, Task>
{
    PRECOND((task_index.underlying() & task_index_tag_mask) == IndexTags::generic);

    return m_generic_work_tree.try_emplace_at(
        WorkIndex{ task_index.underlying() & task_index_value_mask },
        [x_task = std::move(task)] mutable -> WorkContinuation {
            x_task();
            return WorkContinuation::eDontCare;
        }
    );
}

auto ddge::exec::v2::TaskHub::try_emplace_main_only_at(
    Task&&          task,
    const TaskIndex task_index
) -> std::expected<void, Task>
{
    PRECOND((task_index.underlying() & task_index_tag_mask) == IndexTags::main_only);

    return m_main_only_work_tree.try_emplace_at(
        WorkIndex{ task_index.underlying() & task_index_value_mask },
        [x_task = std::move(task)] mutable -> WorkContinuation {
            x_task();
            return WorkContinuation::eDontCare;
        }
    );
}

#define STRINGIFY(x) #x

auto ddge::exec::v2::TaskHub::schedule(const TaskIndex task_index) -> void
{
    if (const TaskIndex::Underlying type{ task_index.underlying() & task_index_tag_mask };
        type == IndexTags::generic)
    {
        m_generic_work_tree.schedule(
            WorkIndex{ task_index.underlying() & task_index_value_mask }
        );
    }
    else if (type == IndexTags::main_only) {
        m_main_only_work_tree.schedule(
            WorkIndex{ task_index.underlying() & task_index_value_mask }
        );
    }
    else {
        PRECOND(false, std::format("invalid {}", STRINGIFY(task_index)));
        std::unreachable();
    }
}

auto ddge::exec::v2::TaskHub::try_execute_a_generic_task(const uint32_t thread_id) -> bool
{
    return m_generic_work_tree.try_execute_one_work(thread_id);
}

auto ddge::exec::v2::TaskHub::try_execute_a_main_only_task() -> bool
{
    return m_main_only_work_tree.try_execute_one_work(0);
}
