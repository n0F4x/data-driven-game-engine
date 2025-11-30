module;

#include <atomic>
#include <cstdint>
#include <memory>
#include <optional>
#include <utility>

#include "utility/contract_macros.hpp"

export module ddge.modules.scheduler.gatherers.WaitAll;

import ddge.modules.scheduler.gatherers.gatherer_of_c;
import ddge.modules.scheduler.gatherers.gatherer_builder_of_c;
import ddge.modules.scheduler.TaskContinuation;
import ddge.modules.scheduler.TaskHubProxy;

import ddge.utility.contracts;

namespace ddge::scheduler {

export class WaitAll {
public:
    using Output = void;

    struct Precondition {
        explicit Precondition(uint32_t capacity);
    };

    explicit WaitAll(uint32_t capacity);

    auto set_continuation(TaskContinuation<void>&& continuation) -> void;
    auto receive() -> void;

private:
    [[no_unique_address]]
    Precondition m_precondition;

    uint32_t                              m_capacity;
    std::atomic_uint32_t                  m_flag;
    std::optional<TaskContinuation<void>> m_continuation;

    auto call_callback() -> void;
};

static_assert(gatherer_of_c<WaitAll, void>);

export class WaitAllBuilder {
public:
    using Result = void;

    [[nodiscard]]
    static auto build(const uint32_t number_of_gathers) -> std::shared_ptr<WaitAll>
    {
        return std::make_shared<WaitAll>(number_of_gathers);
    }
};

static_assert(gatherer_builder_of_c<WaitAllBuilder, void>);

}   // namespace ddge::scheduler

ddge::scheduler::WaitAll::Precondition::Precondition(const uint32_t capacity)
{
    PRECOND(capacity > 0);
}

ddge::scheduler::WaitAll::WaitAll(const uint32_t capacity)
    : m_precondition{ capacity },
      m_capacity{ capacity },
      m_flag{ m_capacity }
{}

auto ddge::scheduler::WaitAll::set_continuation(TaskContinuation<void>&& continuation)
    -> void
{
    m_continuation = std::move(continuation);
}

auto ddge::scheduler::WaitAll::receive() -> void
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

auto ddge::scheduler::WaitAll::call_callback() -> void
{
    if (m_continuation.has_value()) {
        (*m_continuation)();
    }
}
