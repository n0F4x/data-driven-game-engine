export module addons.ECS;

import core.ecs;

import extensions.scheduler.provider_for;
import extensions.scheduler.providers.ECSProvider;

namespace addons {

export struct ECS {
    core::ecs::Registry registry;
};

}   // namespace addons

template <>
struct extensions::scheduler::ProviderFor<addons::ECS> {
    using type = extensions::scheduler::providers::ECSProvider<addons::ECS>;
};
