module;

#include <optional>
#include <utility>

#include "utility/contract_macros.hpp"

export module ddge.modules.states.Plugin;

import ddge.modules.app;
import ddge.modules.states.Addon;
import ddge.modules.states.state_c;
import ddge.modules.store.Store;

import ddge.utility.contracts;

namespace ddge::states {

export class Plugin {
public:
    template <state_c State_T, app::decays_to_builder_c Self_T>
    auto register_state(this Self_T&&) -> Self_T;

    template <app::decays_to_app_c App_T>
    [[nodiscard]]
    auto build(App_T&& app) && -> app::add_on_t<App_T, Addon>;

    template <state_c State_T>
    [[nodiscard]]
    auto contains_state() const noexcept -> bool;

private:
    store::Store m_states;
};

}   // namespace ddge::states

template <ddge::states::state_c State_T, ddge::app::decays_to_builder_c Self_T>
auto ddge::states::Plugin::register_state(this Self_T&& self) -> Self_T
{
    Plugin& this_self{ static_cast<Plugin&>(self) };
    PRECOND((!this_self.contains_state<State_T>()));
    this_self.m_states.emplace<std::optional<State_T>>();
    return std::forward<Self_T>(self);
}

template <ddge::app::decays_to_app_c App_T>
auto ddge::states::Plugin::build(App_T&& app) && -> app::add_on_t<App_T, Addon>
{
    return std::forward<App_T>(app).add_on(Addon{ .state_manager{ std::move(m_states) } });
}

template <ddge::states::state_c State_T>
auto ddge::states::Plugin::contains_state() const noexcept -> bool
{
    return m_states.contains<std::optional<State_T>>();
}
