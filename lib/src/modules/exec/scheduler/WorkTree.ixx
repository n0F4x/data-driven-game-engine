module;

#include <atomic>
#include <expected>
#include <optional>
#include <utility>
#include <vector>

export module ddge.modules.exec.scheduler.WorkTree;

import ddge.modules.exec.scheduler.SignalTree;
import ddge.modules.exec.scheduler.Work;
import ddge.modules.exec.scheduler.WorkContinuation;
import ddge.modules.exec.scheduler.WorkIndex;

import ddge.utility.containers.AnyMoveOnlyFunction;

namespace ddge::exec {

struct WorkFlags {
    using type = uint8_t;

    constexpr static type eNone              = 0;
    constexpr static type eActive            = 0x1 << 0;
    constexpr static type eShouldBeScheduled = 0x1 << 1;
    constexpr static type eShouldBeReleased  = 0x1 << 2;
    constexpr static type eAll               = static_cast<type>(~eNone);
};

export using ReleaseWorkContract = util::AnyMoveOnlyFunction<void()>;

class WorkContract {
public:
    auto assign(Work&& work, std::optional<ReleaseWorkContract>&& release)
        -> WorkContract&;

    [[nodiscard]]
    auto execute() -> WorkContinuation;

    [[nodiscard]]
    auto schedule() -> bool;
    [[nodiscard]]
    auto schedule_release() -> WorkContinuation;

private:
    std::atomic<WorkFlags::type>       m_flags;
    std::optional<Work>                m_work;
    std::optional<ReleaseWorkContract> m_release;

    auto release() -> void;

    static_assert(std::remove_cvref_t<decltype(m_flags)>::is_always_lock_free);
};

export class WorkTree {
public:
    WorkTree(uint64_t capacity, uint32_t number_of_threads);

    [[nodiscard]]
    auto try_emplace(Work&& work) -> std::expected<WorkIndex, Work>;
    [[nodiscard]]
    auto try_emplace(Work&& work, ReleaseWorkContract&& release)
        -> std::expected<WorkIndex, std::pair<Work, ReleaseWorkContract>>;

    [[nodiscard]]
    auto try_emplace_at(WorkIndex work_index, Work&& work) -> std::expected<void, Work>;
    [[nodiscard]]
    auto try_emplace_at(WorkIndex work_index, Work&& work, ReleaseWorkContract&& release)
        -> std::expected<void, std::pair<Work, ReleaseWorkContract>>;

    auto try_execute_one_work(uint32_t thread_id) -> bool;

    auto schedule(WorkIndex work_index) -> void;
    auto schedule_for_release(WorkIndex work_index) -> void;

    [[nodiscard]]
    auto capacity() const noexcept -> uint64_t;
    [[nodiscard]]
    auto optimized_for_thread_count() const noexcept -> uint32_t;

private:
    std::vector<SignalTree>   m_free_signals;
    std::vector<SignalTree>   m_contract_signals;
    std::vector<WorkContract> m_work_contracts;
    std::atomic<uint64_t>     m_next_available_sub_tree_index{};

    [[nodiscard]]
    auto try_emplace(Work&& work, std::optional<ReleaseWorkContract>&& release)
        -> std::expected<WorkIndex, std::pair<Work, std::optional<ReleaseWorkContract>>>;
    [[nodiscard]]
    auto try_emplace_at(
        WorkIndex                            work_index,
        Work&&                               work,
        std::optional<ReleaseWorkContract>&& release
    ) -> std::expected<void, std::pair<Work, std::optional<ReleaseWorkContract>>>;

    auto handle_work_result(WorkIndex work_index, WorkContinuation work_continuation)
        -> void;

    static_assert(
        std::remove_cvref_t<decltype(m_next_available_sub_tree_index)>::is_always_lock_free
    );
};

}   // namespace ddge::exec
