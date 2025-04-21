export module extensions.scheduler.dependency_providers.ecs;

import addons.ecs;

import core.app.has_addons_c;

export import extensions.scheduler.dependency_providers.ecs.DependencyProvider;
export import extensions.scheduler.dependency_providers.ecs.accessors;

namespace extensions::scheduler::dependency_providers {

export struct ECS {
    [[nodiscard]]
    constexpr static auto operator()(core::app::has_addons_c<addons::ECS> auto&)
        -> ecs::DependencyProvider;
};

}   // namespace extensions::scheduler::dependency_providers

constexpr auto extensions::scheduler::dependency_providers::ECS::operator()(
    core::app::has_addons_c<addons::ECS> auto& app
) -> ecs::DependencyProvider
{
    return ecs::DependencyProvider{ app.registry };
}
