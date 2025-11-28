module;

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <utility>

#include "utility/contracts_macros.hpp"

export module ddge.modules.exec.v2.gatherers.AllOf;

import ddge.modules.exec.v2.gatherers.gatherer_of_c;
import ddge.modules.exec.v2.gatherers.gatherer_builder_of_c;
import ddge.modules.exec.v2.TaskContinuation;
import ddge.modules.exec.v2.TaskHubProxy;

import ddge.utility.contracts;

namespace ddge::exec::v2 {

export class AllOf {
public:
    using Output = bool;

    struct Precondition {
        explicit Precondition(uint32_t capacity);
    };

    explicit AllOf(uint32_t capacity);

    auto set_continuation(TaskContinuation<bool>&& continuation) -> void;
    auto receive(bool value) -> void;

private:
    [[no_unique_address]]
    Precondition m_precondition;

    uint32_t                              m_capacity;
    std::atomic_uint32_t                  m_flag;
    std::atomic_bool                      m_accumulated_result;
    std::optional<TaskContinuation<bool>> m_continuation;

    auto call_callback(bool output) -> void;
};

static_assert(gatherer_of_c<AllOf, bool>);

export class AllOfBuilder {
public:
    using Result = bool;

    [[nodiscard]]
    static auto build(const uint32_t number_of_gathers) -> std::shared_ptr<AllOf>
    {
        return std::make_shared<AllOf>(number_of_gathers);
    }
};

static_assert(gatherer_builder_of_c<AllOfBuilder, bool>);

}   // namespace ddge::exec::v2

ddge::exec::v2::AllOf::Precondition::Precondition(const uint32_t capacity)
{
    PRECOND(capacity > 0);
}

ddge::exec::v2::AllOf::AllOf(const uint32_t capacity)
    : m_precondition{ capacity },
      m_capacity{ capacity },
      m_flag{ m_capacity },
      m_accumulated_result{ true }
{}

auto ddge::exec::v2::AllOf::set_continuation(TaskContinuation<bool>&& continuation)
    -> void
{
    m_continuation = std::move(continuation);
}

auto ddge::exec::v2::AllOf::receive(const bool value) -> void
{
    if (m_capacity == 1) {
        call_callback(value);
    }
    else {
        if (value == false) {
            m_accumulated_result.store(false);
        }
        if (m_flag.fetch_sub(1) - 1 == 0) {
            m_flag.store(m_capacity);
            const bool result = m_accumulated_result.exchange(true);
            call_callback(result);
        }
    }
}

auto ddge::exec::v2::AllOf::call_callback(const bool output) -> void
{
    if (m_continuation.has_value()) {
        (*m_continuation)(auto{ output });
    }
}
