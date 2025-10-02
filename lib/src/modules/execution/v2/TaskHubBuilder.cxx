module;

#include <cassert>
#include <expected>
#include <limits>
#include <ranges>
#include <thread>
#include <utility>
#include <vector>

#include <function2/function2.hpp>

#include "utility/contracts_macros.hpp"

module ddge.modules.execution.v2.TaskHubBuilder;

import ddge.modules.execution.v2.TaskHubProxy;

import ddge.utility.contracts;

auto ddge::exec::v2::TaskHubBuilder::emplace(
    fu2::unique_function<void(const TaskHubProxy&)>&& task,
    const ExecPolicy                                  execution_policy
) -> TaskIndex
{
    switch (execution_policy) {
        case ExecPolicy::eDefault: {
            PRECOND(
                m_generic_work_factories.size()
                < (std::numeric_limits<TaskIndex::Underlying>::max() >> 1)
            );

            m_generic_work_factories.push_back(std::move(task));

            return TaskIndex{ m_generic_work_factories.size() - 1 };
        }
        case ExecPolicy::eForceOnMain: {
            PRECOND(
                m_main_only_work_factories.size()
                < (std::numeric_limits<TaskIndex::Underlying>::max() >> 2)
            );

            m_main_only_work_factories.push_back(std::move(task));

            return TaskIndex{
                (m_main_only_work_factories.size() - 1)
                | TaskHub::IndexTags::main_only   //
            };
        }
    }
}

auto ddge::exec::v2::TaskHubBuilder::build() && -> std::unique_ptr<TaskHub>
{
    std::unique_ptr<TaskHub> result{
        std::make_unique<TaskHub>(
            m_generic_work_factories.size(),
            m_main_only_work_factories.size(),
            std::jthread::hardware_concurrency()
        )   //
    };


    for (auto&& [i, task] : std::views::zip(
             std::views::iota(0u, m_generic_work_factories.size()),
             std::views::as_rvalue(std::move(m_generic_work_factories))
         ))
    {
        const std::expected expected = result->try_emplace_generic_at(
            [task_hub_proxy = TaskHubProxy{ *result },
             x_task         = std::move(task)]   //
            () mutable -> void           //
            {                            //
                x_task(task_hub_proxy);
            },
            TaskIndex{ i | TaskHub::IndexTags::generic }
        );
        assert(expected.has_value());
    }

    for (auto&& [i, task] : std::views::zip(
             std::views::iota(0u, m_main_only_work_factories.size()),
             std::views::as_rvalue(std::move(m_main_only_work_factories))
         ))
    {
        const std::expected expected = result->try_emplace_main_only_at(
            [task_hub_proxy = TaskHubProxy{ *result },
             x_task         = std::move(task)]   //
            () mutable -> void           //
            {                            //
                x_task(task_hub_proxy);
            },
            TaskIndex{ i | TaskHub::IndexTags::main_only }
        );
        assert(expected.has_value());
    }


    return result;
}
