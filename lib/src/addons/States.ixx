export module addons.States;

import core.states;

import extensions.scheduler.provider_for;
import extensions.scheduler.providers.StateProvider;

namespace addons {

export struct StatesTag {};

export template <core::states::state_c... States_T>
struct States : StatesTag {
    core::states::StateManager<States_T...> state_manager;
};

}   // namespace addons

template <core::states::state_c... States_T>
struct extensions::scheduler::ProviderFor<addons::States<States_T...>> {
    using type = extensions::scheduler::providers::
        StateProvider<addons::States<States_T...>>;
};
