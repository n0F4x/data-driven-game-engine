export module extensions.scheduler.dependency_providers.ecs;

namespace extensions::scheduler::dependency_providers {

namespace ecs {

class DependencyProvider {};

}   // namespace ecs

export struct ECS {
    [[nodiscard]]
    constexpr static auto operator()(auto&) -> ecs::DependencyProvider;
};

}   // namespace extensions::scheduler::dependency_providers

constexpr auto extensions::scheduler::dependency_providers::ECS::operator()(auto&)
    -> ecs::DependencyProvider
{
    return ecs::DependencyProvider{};
}
