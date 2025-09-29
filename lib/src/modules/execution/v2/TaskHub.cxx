module;

#include <expected>
#include <utility>

#include "utility/contracts_macros.hpp"

module ddge.modules.execution.v2.TaskHub;

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
    Work&&          work,
    const WorkIndex work_index
) -> std::expected<void, Work>
{
    return m_generic_work_tree.try_emplace_at(work_index, std::move(work));
}

auto ddge::exec::v2::TaskHub::try_emplace_main_only_at(
    Work&&          work,
    const WorkIndex work_index
) -> std::expected<void, Work>
{
    return m_main_only_work_tree.try_emplace_at(work_index, std::move(work));
}

auto ddge::exec::v2::TaskHub::schedule(const WorkIndex work_index) -> void
{
    if (const WorkIndex::Underlying type{ work_index.underlying() & work_index_tag_mask };
        type == IndexTags::generic)
    {
        m_generic_work_tree.schedule(work_index);
    }
    else if (type == IndexTags::main_only) {
        m_main_only_work_tree.schedule(
            WorkIndex{ work_index.underlying() - IndexTags::main_only }
        );
    }
    else {
        PRECOND(false, "invalid work index");
        std::unreachable();
    }
}

auto ddge::exec::v2::TaskHub::try_execute_one(const uint32_t thread_id) -> bool
{
    return m_generic_work_tree.try_execute_one_work(thread_id);
}

auto ddge::exec::v2::TaskHub::try_execute_one_main_only_work() -> bool
{
    return m_main_only_work_tree.try_execute_one_work(0);
}
