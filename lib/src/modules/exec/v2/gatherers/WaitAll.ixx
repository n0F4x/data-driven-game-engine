module;

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <utility>

#include "utility/contracts_macros.hpp"

export module ddge.modules.exec.v2.gatherers.WaitAll;

import ddge.modules.exec.v2.gatherers.gatherer_of_c;
import ddge.modules.exec.v2.gatherers.gatherer_builder_of_c;
import ddge.modules.exec.v2.TaskContinuation;
import ddge.modules.exec.v2.TaskFinishedCallback;
import ddge.modules.exec.v2.TaskHubProxy;

import ddge.utility.contracts;

namespace ddge::exec::v2 {

export class WaitAll {
public:
    using Output = void;

    struct Precondition {
        Precondition(uint32_t capacity, const TaskFinishedCallback<void>& callback);
    };

    WaitAll(uint32_t capacity, TaskFinishedCallback<void>&& callback);

    auto set_continuation(TaskContinuation<void>&& continuation) -> void;
    auto set_task_hub_proxy(const TaskHubProxy& task_hub_proxy) -> void;
    auto receive() -> void;

private:
    [[no_unique_address]]
    Precondition m_precondition;

    uint32_t                                                  m_capacity;
    std::atomic_uint32_t                                      m_flag;
    TaskFinishedCallback<void>                                m_callback;
    std::optional<TaskContinuation<void>>                     m_continuation;
    std::optional<std::reference_wrapper<const TaskHubProxy>> m_task_hub_proxy_ref;

    auto call_callback() -> void;
};

static_assert(gatherer_of_c<WaitAll, void>);

export class WaitAllBuilder {
public:
    using Result = void;

    [[nodiscard]]
    static auto
        build(const uint32_t number_of_gathers, TaskFinishedCallback<void>&& callback)
            -> std::shared_ptr<WaitAll>
    {
        return std::make_shared<WaitAll>(number_of_gathers, std::move(callback));
    }
};

static_assert(gatherer_builder_of_c<WaitAllBuilder, void>);

}   // namespace ddge::exec::v2

ddge::exec::v2::WaitAll::Precondition::Precondition(
    const uint32_t capacity,
    const TaskFinishedCallback<void>&
)
{
    PRECOND(capacity > 0);
}

ddge::exec::v2::WaitAll::WaitAll(
    const uint32_t               capacity,
    TaskFinishedCallback<void>&& callback
)
    : m_precondition{ capacity, callback },
      m_capacity{ capacity },
      m_flag{ m_capacity },
      m_callback{ std::move(callback) }
{}

auto ddge::exec::v2::WaitAll::set_continuation(TaskContinuation<void>&& continuation)
    -> void
{
    m_continuation = std::move(continuation);
}

auto ddge::exec::v2::WaitAll::set_task_hub_proxy(const TaskHubProxy& task_hub_proxy)
    -> void
{
    m_task_hub_proxy_ref = task_hub_proxy;
}

auto ddge::exec::v2::WaitAll::receive() -> void
{
    if (m_capacity == 1) {
        call_callback();
    }
    else {
        if (m_flag.fetch_sub(1) - 1 == 0) {
            m_flag.store(m_capacity);
            call_callback();
        }
    }
}

auto ddge::exec::v2::WaitAll::call_callback() -> void
{
    if (m_continuation.has_value()) {
        (*m_continuation)();
    }
    else {
        m_callback(m_task_hub_proxy_ref->get());
    }
}
