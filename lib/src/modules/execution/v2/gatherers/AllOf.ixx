module;

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <utility>

#include <function2/function2.hpp>

#include "utility/contracts_macros.hpp"

export module ddge.modules.execution.v2.gatherers.AllOf;

import ddge.modules.execution.v2.gatherers.gatherer_of_c;
import ddge.modules.execution.v2.gatherers.gatherer_builder_of_c;
import ddge.modules.execution.v2.TaskFinishedCallback;
import ddge.modules.execution.v2.TaskHubProxy;

import ddge.utility.contracts;

namespace ddge::exec::v2 {

export class AllOf {
public:
    struct Precondition {
        Precondition(uint32_t capacity, const TaskFinishedCallback<bool>& callback);
    };

    AllOf(uint32_t capacity, TaskFinishedCallback<bool>&& callback);

    auto set_task_hub_proxy(const TaskHubProxy& task_hub_proxy) -> void;
    auto receive(bool value) -> void;

private:
    [[no_unique_address]]
    Precondition m_precondition;

    uint32_t                                                  m_capacity;
    std::atomic_uint32_t                                      m_flag;
    std::atomic_bool                                          m_accumulated_result;
    TaskFinishedCallback<bool>                                m_callback;
    std::optional<std::reference_wrapper<const TaskHubProxy>> m_task_hub_proxy_ref;
};

static_assert(gatherer_of_c<AllOf, bool>);

export class AllOfBuilder {
public:
    using Result = bool;

    [[nodiscard]]
    static auto
        build(const uint32_t number_of_gathers, TaskFinishedCallback<bool>&& callback)
            -> std::shared_ptr<AllOf>
    {
        return std::make_shared<AllOf>(number_of_gathers, std::move(callback));
    }
};

static_assert(gatherer_builder_of_c<AllOfBuilder, bool>);

}   // namespace ddge::exec::v2

ddge::exec::v2::AllOf::Precondition::
    Precondition(const uint32_t capacity, const TaskFinishedCallback<bool>&)
{
    PRECOND(capacity > 0);
}

ddge::exec::v2::AllOf::AllOf(const uint32_t capacity, TaskFinishedCallback<bool>&& callback)
    : m_precondition{ capacity, callback },
      m_capacity{ capacity },
      m_flag{ m_capacity },
      m_accumulated_result{ true },
      m_callback{ std::move(callback) }
{}

auto ddge::exec::v2::AllOf::set_task_hub_proxy(const TaskHubProxy& task_hub_proxy) -> void
{
    m_task_hub_proxy_ref = task_hub_proxy;
}

auto ddge::exec::v2::AllOf::receive(const bool value) -> void
{
    if (m_capacity == 1) {
        m_callback(m_task_hub_proxy_ref->get(), value);
    }
    else {
        if (value == false) {
            m_accumulated_result.store(false);
        }
        if (m_flag.fetch_sub(1) - 1 == 0) {
            m_flag.store(m_capacity);
            const bool result = m_accumulated_result.exchange(true);
            m_callback(m_task_hub_proxy_ref->get(), result);
        }
    }
}
