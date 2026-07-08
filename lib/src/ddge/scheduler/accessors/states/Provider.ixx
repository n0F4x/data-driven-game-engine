module;

#include <format>
#include <functional>
#include <type_traits>

#include "ddge/util/contract_macros.hpp"

export module ddge.scheduler.accessors.states:Provider;

import :State;

import ddge.app;
import ddge.scheduler.ProviderFor;
import ddge.scheduler.ProviderOf;
import ddge.states;

import ddge.util.contracts;
import ddge.util.meta.concepts.specialization_of;
import ddge.util.meta.reflection.name_of;

namespace ddge::scheduler::accessors {

inline namespace states {

export class Provider {
public:
    template <ddge::app::has_addons_c<ddge::states::Addon> App_T>
    constexpr explicit Provider(App_T& app);

    template <util::meta::specialization_of_c<accessors::states::State> State_T>
    [[nodiscard]]
    constexpr auto provide() const -> State_T;

private:
    std::reference_wrapper<ddge::states::StateManager> m_state_manager_ref;
};

}   // namespace states

}   // namespace ddge::scheduler::accessors

template <>
struct ddge::scheduler::ProviderFor<ddge::states::Addon>
    : std::type_identity<ddge::scheduler::accessors::states::Provider> {};

template <typename State_T>
struct ddge::scheduler::ProviderOf<ddge::scheduler::accessors::states::State<State_T>>
    : std::type_identity<ddge::scheduler::accessors::states::Provider> {};

template <ddge::app::has_addons_c<ddge::states::Addon> App_T>
constexpr ddge::scheduler::accessors::states::Provider::Provider(App_T& app)
    : m_state_manager_ref{ app.state_manager }
{}

template <ddge::util::meta::specialization_of_c<ddge::scheduler::accessors::states::State>
              State_T>
constexpr auto ddge::scheduler::accessors::states::Provider::provide() const -> State_T
{
    using State = std::remove_const_t<typename State_T::Underlying>;

    PRECOND(
        (m_state_manager_ref.get().contains<State>()),
        std::format("State `{}` does not exist", util::meta::name_of<State>())
    );

    return State_T{ m_state_manager_ref.get().at<State>() };
}
