module;

#include <utility>

export module plugins.states;

import app.Builder;
import app.decays_to_app_c;
import app.decays_to_builder_c;
import core.states;

import addons.States;

import utility.meta.type_traits.type_list.type_list_contains;
import utility.TypeList;

namespace plugins {

export struct StatesTag {};

export template <core::states::state_c... States_T>
class BasicStates : public StatesTag {
public:
    template <core::states::state_c State_T, app::decays_to_builder_c Self_T>
        requires(!util::meta::type_list_contains_v<util::TypeList<States_T...>, State_T>)
    constexpr auto register_state(this Self_T&&);

    template <app::decays_to_app_c App_T>
    [[nodiscard]]
    constexpr auto build(App_T&& app);
};

export using States = BasicStates<>;

export inline constexpr States states;

}   // namespace plugins

template <core::states::state_c... States_T>
template <core::states::state_c State_T, app::decays_to_builder_c Self_T>
    requires(!util::meta::type_list_contains_v<util::TypeList<States_T...>, State_T>)
constexpr auto plugins::BasicStates<States_T...>::register_state(
    this Self_T&& self
)
{
    return app::swap_plugin<BasicStates>(
        std::forward<Self_T>(self),
        [](auto&&) { return BasicStates<States_T..., State_T>{}; }
    );
}

template <core::states::state_c... States_T>
template <app::decays_to_app_c App_T>
constexpr auto plugins::BasicStates<States_T...>::build(App_T&& app)
{
    return std::forward<App_T>(app).add_on(addons::States<States_T...>{});
}
