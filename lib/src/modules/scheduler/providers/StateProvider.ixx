module;

#include <format>
#include <functional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module ddge.modules.scheduler.providers.StateProvider;

import ddge.modules.app;
import ddge.modules.states;
import ddge.modules.scheduler.ProviderFor;

import ddge.modules.scheduler.accessors.states;
import ddge.modules.scheduler.ProviderOf;

import ddge.utility.contracts;
import ddge.utility.meta.concepts.specialization_of;
import ddge.utility.meta.reflection.name_of;

namespace ddge::scheduler::providers {

export class StateProvider {
public:
    template <ddge::app::has_addons_c<states::Addon> App_T>
    constexpr explicit StateProvider(App_T& app);

    template <util::meta::specialization_of_c<accessors::states::State> State_T>
    [[nodiscard]]
    constexpr auto provide() const -> State_T;

private:
    std::reference_wrapper<ddge::states::StateManager> m_state_manager_ref;
};

}   // namespace ddge::scheduler::providers

template <>
struct ddge::scheduler::ProviderOf<ddge::states::Addon>
    : std::type_identity<ddge::scheduler::providers::StateProvider> {};

template <typename State_T>
struct ddge::scheduler::ProviderFor<ddge::scheduler::accessors::states::State<State_T>>
    : std::type_identity<ddge::scheduler::providers::StateProvider> {};

template <ddge::app::has_addons_c<ddge::states::Addon> App_T>
constexpr ddge::scheduler::providers::StateProvider::StateProvider(App_T& app)
    : m_state_manager_ref{ app.state_manager }
{}

template <ddge::util::meta::specialization_of_c<ddge::scheduler::accessors::states::State>
              State_T>
constexpr auto ddge::scheduler::providers::StateProvider::provide() const -> State_T
{
    using State = std::remove_const_t<typename State_T::Underlying>;

    PRECOND(
        (m_state_manager_ref.get().contains<State>()),
        std::format("State `{}` does not exist", util::meta::name_of<State>())
    );

    return State_T{ m_state_manager_ref.get().at<State>() };
}
