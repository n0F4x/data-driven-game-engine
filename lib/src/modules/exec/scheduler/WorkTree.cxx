module;

#include <algorithm>
#include <atomic>
#include <bit>
#include <cassert>
#include <expected>
#include <optional>
#include <ranges>
#include <utility>

#include "utility/contracts_macros.hpp"

module ddge.modules.exec.scheduler.WorkTree;

import ddge.modules.exec.scheduler.SignalTree;
import ddge.modules.exec.scheduler.Work;
import ddge.modules.exec.scheduler.WorkContinuation;

import ddge.utility.contracts;

auto ddge::exec::WorkContract::assign(
    Work&&                               work,
    std::optional<ReleaseWorkContract>&& release
) -> WorkContract&
{
    m_work    = std::move(work);
    m_release = std::move(release);
    m_flags.store(WorkFlags::eNone);

    return *this;
}

auto ddge::exec::WorkContract::execute() -> WorkContinuation
{
    if (const bool should_be_released{ (m_flags.load() & WorkFlags::eShouldBeReleased)
                                       != 0 };
        should_be_released)
    {
        release();
        return WorkContinuation::eRelease;
    }

    m_flags.fetch_and(WorkFlags::eAll - WorkFlags::eShouldBeScheduled);

    assert(m_work.has_value());
    const WorkContinuation continuation = m_work->operator()();

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
            if (m_release.has_value()) {
                m_flags.fetch_or(WorkFlags::eShouldBeReleased);
                return WorkContinuation::eReschedule;
            }
            return WorkContinuation::eRelease;
    }
}

auto ddge::exec::WorkContract::schedule() -> bool
{
    return (m_flags.fetch_or(WorkFlags::eActive & WorkFlags::eShouldBeScheduled)
            & WorkFlags::eActive)
        == 0;
}

auto ddge::exec::WorkContract::schedule_release() -> WorkContinuation
{
    if (!m_release.has_value()) {
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

    m_flags.fetch_or(WorkFlags::eShouldBeReleased);
    return schedule() ? WorkContinuation::eReschedule : WorkContinuation::eDontCare;
}

auto ddge::exec::WorkContract::release() -> void
{
    if (m_release.has_value()) {
        m_release->operator()();
    }
}

[[nodiscard]]
constexpr auto real_capacity_per_sub_tree(
    const uint64_t desired_capacity,
    const uint32_t number_of_threads
) -> uint64_t
{
    return std::max(
        desired_capacity / number_of_threads
            + std::min(desired_capacity % number_of_threads, uint64_t{ 1 }),
        static_cast<uint64_t>(ddge::exec::SignalTree::minimum_number_of_levels())
    );
}

[[nodiscard]]
constexpr auto necessary_number_of_signal_tree_levels(
    const uint64_t number_of_work_contracts,
    const uint32_t number_of_threads
) -> uint32_t
{
    const auto capacity{
        real_capacity_per_sub_tree(number_of_work_contracts, number_of_threads)
    };

    return std::max(
        static_cast<uint32_t>(
            std::bit_width(capacity) + (std::has_single_bit(capacity) ? 0 : 1)
        ),
        ddge::exec::SignalTree::minimum_number_of_levels()
    );
}

ddge::exec::WorkTree::WorkTree(const uint64_t capacity, const uint32_t number_of_threads)
    : m_work_contracts{ real_capacity_per_sub_tree(capacity, number_of_threads)
                        * number_of_threads }
{
    PRECOND(number_of_threads > 0);

    m_free_signals.reserve(number_of_threads);
    m_contract_signals.reserve(number_of_threads);
    for (auto _ : std::views::repeat(std::ignore, number_of_threads)) {
        m_free_signals.emplace_back(
            necessary_number_of_signal_tree_levels(capacity, number_of_threads)
        );
        m_contract_signals.emplace_back(
            necessary_number_of_signal_tree_levels(capacity, number_of_threads)
        );
    }

    for (SignalTree& signal_tree : m_free_signals) {
        for (const uint32_t index : std::views::iota(0u, signal_tree.number_of_leaves()))
        {
            signal_tree.try_set_one(index);
        }
    }
}

auto ddge::exec::WorkTree::try_emplace(Work&& work) -> std::expected<WorkIndex, Work>
{
    return try_emplace(std::move(work), std::nullopt)
        .transform_error(
            [](std::pair<Work, std::optional<ReleaseWorkContract>>&& pair) -> Work {
                return std::move(pair.first);
            }
        );
}

auto ddge::exec::WorkTree::try_emplace(Work&& work, ReleaseWorkContract&& release)
    -> std::expected<WorkIndex, std::pair<Work, ReleaseWorkContract>>
{
    return try_emplace(std::move(work), std::optional{ std::move(release) })
        .transform_error(
            [](std::pair<Work, std::optional<ReleaseWorkContract>>&& pair
            ) -> std::pair<Work, ReleaseWorkContract> {
                assert(pair.second.has_value());
                return std::make_pair(std::move(pair.first), std::move(*pair.second));
            }
        );
}

auto ddge::exec::WorkTree::try_emplace_at(const WorkIndex work_index, Work&& work)
    -> std::expected<void, Work>
{
    return try_emplace_at(work_index, std::move(work), std::nullopt)
        .transform_error(
            [](std::pair<Work, std::optional<ReleaseWorkContract>>&& pair) -> Work {
                return std::move(pair.first);
            }
        );
}

auto ddge::exec::WorkTree::try_emplace_at(
    const WorkIndex       work_index,
    Work&&                work,
    ReleaseWorkContract&& release
) -> std::expected<void, std::pair<Work, ReleaseWorkContract>>
{
    return try_emplace_at(work_index, std::move(work), std::optional{ std::move(release) })
        .transform_error(
            [](std::pair<Work, std::optional<ReleaseWorkContract>>&& pair
            ) -> std::pair<Work, ReleaseWorkContract> {
                assert(pair.second.has_value());
                return std::make_pair(std::move(pair.first), std::move(*pair.second));
            }
        );
}

[[nodiscard]]
constexpr auto default_strategy(uint32_t) -> ddge::exec::TravelsalBias
{
    return ddge::exec::TravelsalBias::eLeft;
}

auto ddge::exec::WorkTree::try_execute_one_work(const uint32_t thread_id) -> bool
{
    PRECOND(
        thread_id < m_contract_signals.size(),
        "Thread id must be smaller than the number of threads the workhub is intended for"
    );

    for (const uint32_t offset : std::views::iota(0u, m_contract_signals.size())) {
        const uint32_t sub_tree_index{
            static_cast<uint32_t>((thread_id + offset) % m_contract_signals.size())
        };

        const std::optional<WorkIndex> work_index{
            m_contract_signals[sub_tree_index]
                // TODO: use per thread strategy
                .try_unset_one(default_strategy)
                .transform([this,
                            sub_tree_index](const SignalTree::LeafIndex signal_index) {
                    return signal_index * m_contract_signals.size() + sub_tree_index;
                })
        };

        if (!work_index.has_value()) {
            continue;
        }

        const WorkContinuation continuation =
            m_work_contracts[work_index->underlying()].execute();

        handle_work_result(*work_index, continuation);

        return true;
    }

    return false;
}

auto ddge::exec::WorkTree::schedule(const WorkIndex work_index) -> void
{
    if (m_work_contracts[work_index.underlying()].schedule()) {
        m_contract_signals[work_index.underlying() % m_contract_signals.size()].try_set_one(
            static_cast<SignalTree::LeafIndex>(
                work_index.underlying() / m_contract_signals.size()
            )
        );
    }
}

auto ddge::exec::WorkTree::schedule_for_release(const WorkIndex work_index) -> void
{
    const WorkContinuation work_continuation{
        m_work_contracts[work_index.underlying()].schedule_release()
    };

    handle_work_result(work_index, work_continuation);
}

auto ddge::exec::WorkTree::capacity() const noexcept -> uint64_t
{
    return m_work_contracts.size();
}

auto ddge::exec::WorkTree::optimized_for_thread_count() const noexcept -> uint32_t
{
    return static_cast<uint32_t>(m_contract_signals.size());
}

auto ddge::exec::WorkTree::try_emplace(
    Work&&                               work,
    std::optional<ReleaseWorkContract>&& release
) -> std::expected<WorkIndex, std::pair<Work, std::optional<ReleaseWorkContract>>>
{
    for (auto _ : std::views::repeat(std::ignore, m_free_signals.size())) {
        const auto sub_tree_index{
            m_next_available_sub_tree_index.fetch_add(1, std::memory_order::relaxed)
            % m_free_signals.size()
        };

        const std::optional<WorkIndex> work_index{
            m_free_signals[sub_tree_index]
                // TODO: try reserving latest
                .try_unset_one(default_strategy)
                .transform([this,
                            sub_tree_index](const SignalTree::LeafIndex signal_index) {
                    return signal_index * m_free_signals.size() + sub_tree_index;
                })
        };

        if (!work_index.has_value()) {
            continue;
        }

        m_work_contracts[work_index->underlying()].assign(
            std::move(work), std::move(release)
        );

        return *work_index;
    }

    return std::expected<WorkIndex, std::pair<Work, std::optional<ReleaseWorkContract>>>{
        std::unexpect, std::move(work), std::move(release)
    };
}

auto ddge::exec::WorkTree::try_emplace_at(
    const WorkIndex                      work_index,
    Work&&                               work,
    std::optional<ReleaseWorkContract>&& release
) -> std::expected<void, std::pair<Work, std::optional<ReleaseWorkContract>>>
{
    const auto sub_tree_index{ work_index.underlying() % m_free_signals.size() };

    const bool success{ m_free_signals[sub_tree_index].try_unset_one_at(
        static_cast<SignalTree::LeafIndex>(work_index.underlying() / m_free_signals.size())
    ) };

    if (!success) {
        return std::expected<void, std::pair<Work, std::optional<ReleaseWorkContract>>>{
            std::unexpect, std::move(work), std::move(release)
        };
    }

    m_work_contracts[work_index.underlying()].assign(std::move(work), std::move(release));

    return std::expected<void, std::pair<Work, std::optional<ReleaseWorkContract>>>{};
}

auto ddge::exec::WorkTree::handle_work_result(
    const WorkIndex        work_index,
    const WorkContinuation work_continuation
) -> void
{
    switch (work_continuation) {
        case WorkContinuation::eDontCare: break;
        case WorkContinuation::eReschedule:
            m_contract_signals[work_index.underlying() % m_contract_signals.size()]
                .try_set_one(
                    static_cast<SignalTree::LeafIndex>(
                        work_index.underlying() / m_contract_signals.size()
                    )
                );
            break;
        case WorkContinuation::eRelease:
            m_free_signals[work_index.underlying() % m_free_signals.size()].try_set_one(
                static_cast<SignalTree::LeafIndex>(
                    work_index.underlying() / m_free_signals.size()
                )
            );
            break;
    }
}
