module;

#include <atomic>
#include <expected>
#include <functional>
#include <utility>
#include <vector>

#include <function2/function2.hpp>

#include "utility/lifetime_bound.hpp"

export module ddge.modules.execution.scheduler.WorkHub;

import ddge.modules.execution.scheduler.SignalTree;
import ddge.modules.execution.scheduler.Work;
import ddge.modules.execution.scheduler.WorkContinuation;

namespace ddge::exec {

export enum struct SizeCategory : uint32_t {
    eDefault = 12,
};

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
    auto schedule_release() -> bool;

private:
    std::atomic<WorkFlags::type> m_flags;
    Work                         m_work;
    ReleaseWorkContract          m_release;
};

using WorkIndex = SignalIndex;

export class WorkHandle;

export class WorkHub {
public:
    explicit WorkHub(SizeCategory size_category);

    [[nodiscard]]
    auto reserve_slot(Work&& work) -> std::expected<WorkHandle, Work>;
    [[nodiscard]]
    auto reserve_slot(Work&& work, ReleaseWorkContract&& release)
        -> std::expected<WorkHandle, std::pair<Work, ReleaseWorkContract>>;

    auto try_execute_one_work() -> bool;

private:
    friend WorkHandle;

    SignalTree                m_free_signals;
    SignalTree                m_contract_signals;
    std::vector<WorkContract> m_work_contracts;

    auto schedule(WorkIndex work_index) -> void;
    auto schedule_release(WorkIndex work_index) -> void;
};

export class WorkHandle {
public:
    explicit WorkHandle([[lifetime_bound]] WorkHub& work_hub, WorkIndex work_index);

    auto schedule() const -> void;
    auto release() const -> void;

private:
    std::reference_wrapper<WorkHub> m_work_hub_ref;
    WorkIndex                       m_work_index;
};

}   // namespace ddge::exec
