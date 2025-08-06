module;

#include <optional>
#include <utility>

#include "utility/contracts_macros.hpp"

export module plugins.states;

import addons.States;

import app;

import core.states;
import core.store.Store;

import utility.contracts;

namespace plugins {

export class States {
public:
    template <core::states::state_c State_T, app::decays_to_builder_c Self_T>
    constexpr auto register_state(this Self_T&&) -> Self_T;

    template <app::decays_to_app_c App_T>
    [[nodiscard]]
    constexpr auto build(App_T&& app) && -> app::add_on_t<App_T, addons::States>;

    template <core::states::state_c State_T>
    [[nodiscard]]
    auto contains_state() const noexcept -> bool;

private:
    core::store::Store m_states;
};

}   // namespace plugins

template <core::states::state_c State_T, app::decays_to_builder_c Self_T>
constexpr auto plugins::States::register_state(this Self_T&& self) -> Self_T
{
    States& this_self{ static_cast<States&>(self) };
    PRECOND((!this_self.contains_state<State_T>()));
    this_self.m_states.emplace<std::optional<State_T>>();
    return std::forward<Self_T>(self);
}

template <app::decays_to_app_c App_T>
constexpr auto plugins::States::build(
    App_T&& app
) && -> app::add_on_t<App_T, addons::States>
{
    return std::forward<App_T>(app).add_on(
        addons::States{
            .state_manager = core::states::StateManager{ std::move(m_states) } }
    );
}

template <core::states::state_c State_T>
auto plugins::States::contains_state() const noexcept -> bool
{
    return m_states.contains<std::optional<State_T>>();
}
