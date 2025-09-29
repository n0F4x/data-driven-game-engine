module;

#include <cassert>
#include <expected>
#include <limits>
#include <ranges>
#include <thread>
#include <utility>

#include <function2/function2.hpp>

#include "utility/contracts_macros.hpp"

module ddge.modules.execution.v2.TaskHubBuilder;

import ddge.modules.execution.scheduler.WorkContinuation;
import ddge.modules.execution.v2.TaskHubProxy;

import ddge.utility.contracts;

auto ddge::exec::v2::TaskHubBuilder::emplace(
    fu2::unique_function<void(TaskHubProxy&)>&& task,
    const ExecPolicy                            execution_policy
) -> WorkIndex
{
    switch (execution_policy) {
        case ExecPolicy::eDefault: {
            PRECOND(
                m_generic_work_factories.size()
                < (std::numeric_limits<WorkIndex::Underlying>::max() >> 1)
            );
            m_generic_work_factories.push_back(std::move(task));
            return WorkIndex{ m_generic_work_factories.size() - 1 };
        }
        case ExecPolicy::eForceOnMain: {
            PRECOND(
                m_main_only_work_factories.size()
                < (std::numeric_limits<WorkIndex::Underlying>::max() >> 2)
            );
            m_main_only_work_factories.push_back(std::move(task));
            return WorkIndex{ (~(std::numeric_limits<WorkIndex::Underlying>::max() >> 2)
                               - ~(std::numeric_limits<WorkIndex::Underlying>::max() >> 1))
                              + m_main_only_work_factories.size() - 1 };
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
             std::views::as_rvalue(m_generic_work_factories)
         ))
    {
        const std::expected expected = result->try_emplace_generic_at(
            [task_hub_proxy = TaskHubProxy{ *result },
             x_task         = std::move(task)]       //
            () mutable -> WorkContinuation   //
            {
                x_task(task_hub_proxy);
                return WorkContinuation::eDontCare;
            },
            WorkIndex{ i }
        );
        assert(expected.has_value());
    }

    for (auto&& [i, task] : std::views::zip(
             std::views::iota(0u, m_main_only_work_factories.size()),
             std::views::as_rvalue(m_main_only_work_factories)
         ))
    {
        const std::expected expected = result->try_emplace_main_only_at(
            [task_hub_proxy = TaskHubProxy{ *result },
             x_task         = std::move(task)]       //
            () mutable -> WorkContinuation   //
            {
                x_task(task_hub_proxy);
                return WorkContinuation::eDontCare;
            },
            WorkIndex{ i }
        );
        assert(expected.has_value());
    }


    return result;
}
