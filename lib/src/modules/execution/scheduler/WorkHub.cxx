module;

#include <expected>
#include <ranges>
#include <utility>

#include "utility/contracts_macros.hpp"

module ddge.modules.execution.scheduler.WorkHub;

import ddge.modules.execution.scheduler.SignalTree;
import ddge.modules.execution.scheduler.Work;
import ddge.modules.execution.scheduler.WorkContinuation;

import ddge.utility.contracts;
import ddge.utility.no_op;

// TODO: Clang can't recognize name "WorkContract" unless its in an open namespace
namespace ddge::exec {

auto WorkContract::assign(Work&& work, ReleaseWorkContract&& release) -> WorkContract&
{
    PRECOND(!work.empty());
    PRECOND(!release.empty());

    m_work    = std::move(work);
    m_release = std::move(release);
    m_flags   = WorkFlags::eNone;

    return *this;
}

auto WorkContract::execute() -> WorkContinuation
{
    if (const bool should_be_released{ (m_flags & WorkFlags::eShouldBeReleased) != 0 };
        should_be_released)
    {
        m_release();
        return WorkContinuation::eRelease;
    }

    m_flags &= (WorkFlags::eAll - WorkFlags::eShouldBeScheduled);

    const WorkContinuation continuation = m_work();

    switch (continuation) {
        case WorkContinuation::eDontCare:   //
            if ((m_flags.fetch_and(WorkFlags::eAll - WorkFlags::eActive)
                 & WorkFlags::eShouldBeScheduled)
                != 0)
            {
                return WorkContinuation::eReschedule;
            }
        case WorkContinuation::eReschedule:   //
            return WorkContinuation::eReschedule;
        case WorkContinuation::eRelease:
            m_flags |= WorkFlags::eShouldBeReleased;
            return WorkContinuation::eReschedule;
    }

    return WorkContinuation::eDontCare;
}

auto WorkContract::schedule() -> bool
{
    return (m_flags.fetch_or(WorkFlags::eActive & WorkFlags::eShouldBeScheduled)
            & WorkFlags::eActive)
        == 0;
}

auto WorkContract::schedule_release() -> bool
{
    m_flags |= WorkFlags::eShouldBeReleased;
    return schedule();
}

}   // namespace ddge::exec

ddge::exec::WorkHub::WorkHub(const SizeCategory size_category)
    : m_free_signals{ std::to_underlying(size_category) },
      m_contract_signals{ std::to_underlying(size_category) },
      m_work_contracts(m_contract_signals.capacity())
{
    for (const SignalIndex index : std::views::iota(0u, m_free_signals.capacity())) {
        m_free_signals.set(index);
    }
}

auto ddge::exec::WorkHub::reserve_slot(Work&& work) -> std::expected<WorkHandle, Work>
{
    return reserve_slot(std::move(work), util::no_op)
        .transform_error([](std::pair<Work, ReleaseWorkContract>&& pair) -> Work {
            return std::move(pair.first);
        });
}

[[nodiscard]]
constexpr auto default_strategy(uint32_t) -> ddge::exec::TravelsalBias
{
    return ddge::exec::TravelsalBias::eLeft;
}

auto ddge::exec::WorkHub::reserve_slot(Work&& work, ReleaseWorkContract&& release)
    -> std::expected<WorkHandle, std::pair<Work, ReleaseWorkContract>>
{
    PRECOND(!work.empty());

    // TODO: try reserving latest
    const std::optional<SignalIndex> signal_index{
        m_free_signals.try_unset_one(default_strategy)
    };

    if (!signal_index.has_value()) {
        return std::expected<WorkHandle, std::pair<Work, ReleaseWorkContract>>{
            std::unexpect, std::move(work), std::move(release)
        };
    }

    m_work_contracts[*signal_index].assign(std::move(work), std::move(release));

    return WorkHandle{ *this, *signal_index };
}

auto ddge::exec::WorkHub::try_execute_one_work() -> bool
{
    // TODO: use per thread strategy
    const std::optional<SignalIndex> signal_index{
        m_contract_signals.try_unset_one(default_strategy)
    };

    if (!signal_index.has_value()) {
        return false;
    }

    const WorkContinuation continuation = m_work_contracts[*signal_index].execute();

    switch (continuation) {
        case WorkContinuation::eDontCare:   break;
        case WorkContinuation::eRelease:    m_free_signals.set(*signal_index); break;
        case WorkContinuation::eReschedule: m_contract_signals.set(*signal_index); break;
    }

    return true;
}

auto ddge::exec::WorkHub::schedule(const WorkIndex work_index) -> void
{
    if (m_work_contracts[work_index].schedule()) {
        m_contract_signals.set(work_index);
    }
}

auto ddge::exec::WorkHub::schedule_release(const WorkIndex work_index) -> void
{
    if (m_work_contracts[work_index].schedule_release()) {
        m_contract_signals.set(work_index);
    }
}

ddge::exec::WorkHandle::WorkHandle(WorkHub& work_hub, const WorkIndex work_index)
    : m_work_hub_ref{ work_hub },
      m_work_index{ work_index }
{}

auto ddge::exec::WorkHandle::schedule() const -> void
{
    m_work_hub_ref.get().schedule(m_work_index);
}

auto ddge::exec::WorkHandle::release() const -> void
{
    m_work_hub_ref.get().schedule_release(m_work_index);
}
