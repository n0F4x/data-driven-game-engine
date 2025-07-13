export module addons.States;

import core.states;

import extensions.scheduler.argument_provider_for;
import extensions.scheduler.argument_providers.StateProvider;

namespace addons {

export struct StatesTag {};

export template <core::states::state_c... States_T>
struct States : StatesTag {
    core::states::StateManager<States_T...> state_manager;
};

}   // namespace addons

template <core::states::state_c... States_T>
struct extensions::scheduler::ArgumentProviderFor<addons::States<States_T...>> {
    using type = extensions::scheduler::argument_providers::
        StateProvider<addons::States<States_T...>>;
};
