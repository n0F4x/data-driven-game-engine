export module extensions.scheduler.argument_providers.ecs;

import addons.ECS;

import app.has_addons_c;

export import extensions.scheduler.argument_providers.ecs.ECSProvider;

namespace extensions::scheduler::argument_providers {

export struct ECSProviderClosure {
    [[nodiscard]]
    constexpr static auto operator()(app::has_addons_c<addons::ECS> auto&)
        -> ECSProvider;
};

export constexpr ECSProviderClosure ecs;

}   // namespace extensions::scheduler::argument_providers

constexpr auto extensions::scheduler::argument_providers::ECSProviderClosure::operator()(
    app::has_addons_c<addons::ECS> auto& app
) -> ECSProvider
{
    return ECSProvider{ app.registry };
}
