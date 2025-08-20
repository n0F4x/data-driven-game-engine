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
        release();
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
            return WorkContinuation::eDontCare;
        case WorkContinuation::eReschedule:   //
            return WorkContinuation::eReschedule;
        case WorkContinuation::eRelease:
            if (!m_release.empty()) {
                m_flags |= WorkFlags::eShouldBeReleased;
                return WorkContinuation::eReschedule;
            }
            return WorkContinuation::eRelease;
    }
}

auto WorkContract::schedule() -> bool
{
    return (m_flags.fetch_or(WorkFlags::eActive & WorkFlags::eShouldBeScheduled)
            & WorkFlags::eActive)
        == 0;
}

auto WorkContract::schedule_release() -> WorkContinuation
{
    if (m_release.empty()) {
        const bool should_be_released =
            (m_flags.fetch_or(WorkFlags::eActive & WorkFlags::eShouldBeReleased)
             & WorkFlags::eActive)
            == 0;
        if (should_be_released) {
            release();
            return WorkContinuation::eRelease;
        }
        return WorkContinuation::eDontCare;
    }

    m_flags |= WorkFlags::eShouldBeReleased;
    return schedule() ? WorkContinuation::eReschedule : WorkContinuation::eDontCare;
}

auto WorkContract::release() -> void
{
    if (!m_release.empty()) {
        m_release();
    }
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
    return reserve_slot(std::move(work), nullptr)
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
    const std::optional<WorkIndex> work_index{
        m_free_signals.try_unset_one(default_strategy)
    };

    if (!work_index.has_value()) {
        return std::expected<WorkHandle, std::pair<Work, ReleaseWorkContract>>{
            std::unexpect, std::move(work), std::move(release)
        };
    }

    m_work_contracts[*work_index].assign(std::move(work), std::move(release));

    return WorkHandle{ *this, *work_index };
}

auto ddge::exec::WorkHub::try_execute_one_work() -> bool
{
    // TODO: use per thread strategy
    const std::optional<WorkIndex> work_index{
        m_contract_signals.try_unset_one(default_strategy)
    };

    if (!work_index.has_value()) {
        return false;
    }

    const WorkContinuation continuation = m_work_contracts[*work_index].execute();

    handle_work_result(*work_index, continuation);

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
    const WorkContinuation work_continuation{
        m_work_contracts[work_index].schedule_release()
    };

    handle_work_result(work_index, work_continuation);
}

auto ddge::exec::WorkHub::handle_work_result(
    const WorkIndex        work_index,
    const WorkContinuation work_continuation
) -> void
{
    switch (work_continuation) {
        case WorkContinuation::eDontCare:   break;
        case WorkContinuation::eReschedule: m_contract_signals.set(work_index); break;
        case WorkContinuation::eRelease:    m_free_signals.set(work_index); break;
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
