module;

#include <atomic>
#include <expected>
#include <utility>
#include <vector>

#include <function2/function2.hpp>

export module ddge.modules.execution.scheduler.WorkTree;

import ddge.modules.execution.scheduler.SignalTree;
import ddge.modules.execution.scheduler.Work;
import ddge.modules.execution.scheduler.WorkContinuation;

import ddge.utility.containers.Strong;

namespace ddge::exec {

struct WorkFlags {
    using type = uint8_t;

    constexpr static type eNone              = 0;
    constexpr static type eActive            = 0x1 << 0;
    constexpr static type eShouldBeScheduled = 0x1 << 1;
    constexpr static type eShouldBeReleased  = 0x1 << 2;
    constexpr static type eAll               = static_cast<type>(~eNone);
};

export using ReleaseWorkContract = fu2::unique_function<void()>;

class WorkContract {
public:
    auto assign(Work&& work, ReleaseWorkContract&& release) -> WorkContract&;

    [[nodiscard]]
    auto execute() -> WorkContinuation;

    [[nodiscard]]
    auto schedule() -> bool;
    [[nodiscard]]
    auto schedule_release() -> WorkContinuation;

private:
    std::atomic<WorkFlags::type> m_flags;
    Work                         m_work;
    ReleaseWorkContract          m_release;

    auto release() -> void;
};

export struct WorkIndex : util::Strong<uint64_t, WorkIndex> {
    using Strong::Strong;
};

export class WorkTree {
public:
    WorkTree(uint64_t capacity, uint32_t number_of_threads);

    [[nodiscard]]
    auto reserve_slot(Work&& work) -> std::expected<WorkIndex, Work>;
    [[nodiscard]]
    auto reserve_slot(Work&& work, ReleaseWorkContract&& release)
        -> std::expected<WorkIndex, std::pair<Work, ReleaseWorkContract>>;

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

    auto handle_work_result(WorkIndex work_index, WorkContinuation work_continuation)
        -> void;
};

}   // namespace ddge::exec
