export module addons.ECS;

import core.ecs;

import extensions.scheduler.argument_provider_for;
import extensions.scheduler.argument_providers.ECSProvider;

namespace addons {

export struct ECS {
    core::ecs::Registry registry;
};

}   // namespace addons

template <>
struct extensions::scheduler::ArgumentProviderFor<addons::ECS> {
    using type = extensions::scheduler::argument_providers::ECSProvider<addons::ECS>;
};
