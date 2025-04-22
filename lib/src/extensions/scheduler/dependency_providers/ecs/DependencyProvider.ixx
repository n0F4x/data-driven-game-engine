module;

#include <functional>

export module extensions.scheduler.dependency_providers.ecs.DependencyProvider;

import core.ecs;

import extensions.scheduler.accessors.ecs;

import utility.meta.concepts.specialization_of;

namespace extensions::scheduler::dependency_providers::ecs {

export class DependencyProvider {
public:
    explicit DependencyProvider(core::ecs::Registry& registry);

    template <util::meta::specialization_of_c<accessors::ecs::Query> Query_T>
    [[nodiscard]]
    auto provide() const -> Query_T;

private:
    std::reference_wrapper<core::ecs::Registry> m_registry;
};

}   // namespace extensions::scheduler::dependency_providers::ecs

extensions::scheduler::dependency_providers::ecs::DependencyProvider::DependencyProvider(
    core::ecs::Registry& registry
)
    : m_registry{ registry }
{}

template <
    util::meta::specialization_of_c<extensions::scheduler::accessors::ecs::Query> Query_T>
auto extensions::scheduler::dependency_providers::ecs::DependencyProvider::provide() const
    -> Query_T
{
    return Query_T{ m_registry };
}
