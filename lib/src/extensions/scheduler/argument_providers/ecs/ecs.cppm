export module extensions.scheduler.argument_providers.ecs;

import addons.ecs;

import core.app.has_addons_c;

export import extensions.scheduler.argument_providers.ecs.ArgumentProvider;

namespace extensions::scheduler::argument_providers {

export struct ECS {
    [[nodiscard]]
    constexpr static auto operator()(core::app::has_addons_c<addons::ECS> auto&)
        -> ecs::ArgumentProvider;
};

}   // namespace extensions::scheduler::argument_providers

constexpr auto extensions::scheduler::argument_providers::ECS::operator()(
    core::app::has_addons_c<addons::ECS> auto& app
) -> ecs::ArgumentProvider
{
    return ecs::ArgumentProvider{ app.registry };
}
