module;

#include <format>
#include <functional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module extensions.scheduler.providers.StateProvider;

import addons.States;

import app;

import core.states;
import core.scheduler.ProviderFor;

import extensions.scheduler.accessors.states;
import extensions.scheduler.ProviderOf;

import utility.contracts;
import utility.meta.concepts.specialization_of;
import utility.meta.reflection.name_of;

namespace extensions::scheduler::providers {

export class StateProvider {
public:
    template <app::has_addons_c<addons::States> App_T>
    constexpr explicit StateProvider(App_T& app);

    template <util::meta::specialization_of_c<accessors::states::State> State_T>
    [[nodiscard]]
    constexpr auto provide() const -> State_T;

private:
    std::reference_wrapper<core::states::StateManager> m_state_manager_ref;
};

}   // namespace extensions::scheduler::providers

template <>
struct extensions::scheduler::ProviderOf<addons::States>
    : std::type_identity<extensions::scheduler::providers::StateProvider> {};

template <typename State_T>
struct core::scheduler::
    ProviderFor<extensions::scheduler::accessors::states::State<State_T>>
    : std::type_identity<extensions::scheduler::providers::StateProvider> {};

template <app::has_addons_c<addons::States> App_T>
constexpr extensions::scheduler::providers::StateProvider::StateProvider(App_T& app)
    : m_state_manager_ref{ app.state_manager }
{}

template <util::meta::specialization_of_c<extensions::scheduler::accessors::states::State>
              State_T>
constexpr auto extensions::scheduler::providers::StateProvider::provide() const -> State_T
{
    using State = std::remove_const_t<typename State_T::Underlying>;

    PRECOND(
        (m_state_manager_ref.get().contains<State>()),
        std::format("State `{}` does not exist", util::meta::name_of<State>())
    );

    return State_T{ m_state_manager_ref.get().at<State>() };
}
