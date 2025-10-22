module;

#include <format>
#include <functional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module ddge.modules.exec.providers.StateProvider;

import ddge.modules.app;
import ddge.modules.states;
import ddge.modules.exec.ProviderFor;

import ddge.modules.exec.accessors.states;
import ddge.modules.exec.ProviderOf;

import ddge.utility.contracts;
import ddge.utility.meta.concepts.specialization_of;
import ddge.utility.meta.reflection.name_of;

namespace ddge::exec::providers {

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

}   // namespace ddge::exec::providers

template <>
struct ddge::exec::ProviderFor<ddge::states::Addon>
    : std::type_identity<ddge::exec::providers::StateProvider> {};

template <typename State_T>
struct ddge::exec::ProviderOf<ddge::exec::accessors::states::State<State_T>>
    : std::type_identity<ddge::exec::providers::StateProvider> {};

template <ddge::app::has_addons_c<ddge::states::Addon> App_T>
constexpr ddge::exec::providers::StateProvider::StateProvider(App_T& app)
    : m_state_manager_ref{ app.state_manager }
{}

template <ddge::util::meta::specialization_of_c<ddge::exec::accessors::states::State>
              State_T>
constexpr auto ddge::exec::providers::StateProvider::provide() const -> State_T
{
    using State = std::remove_const_t<typename State_T::Underlying>;

    PRECOND(
        (m_state_manager_ref.get().contains<State>()),
        std::format("State `{}` does not exist", util::meta::name_of<State>())
    );

    return State_T{ m_state_manager_ref.get().at<State>() };
}
