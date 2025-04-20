module;

#include <functional>

export module extensions.scheduler.dependency_providers.ecs;

import addons.ecs;

import core.app.has_addons_c;
import core.ecs;

import utility.meta.concepts.specialization_of;

namespace extensions::scheduler::dependency_providers {

inline namespace tags {

namespace ecs {

export template <core::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
class Query {
public:
    explicit Query(core::ecs::Registry& registry);

    template <typename F>
    auto for_each(F&& func) const -> F;

private:
    std::reference_wrapper<core::ecs::Registry> m_registry;
};

}   // namespace ecs

}   // namespace tags

namespace ecs {

class DependencyProvider {
public:
    explicit DependencyProvider(core::ecs::Registry& registry);

    template <util::meta::specialization_of_c<tags::ecs::Query> Query_T>
    [[nodiscard]]
    auto provide() const -> Query_T;

private:
    std::reference_wrapper<core::ecs::Registry> m_registry;
};

}   // namespace ecs

export struct ECS {
    [[nodiscard]]
    constexpr static auto operator()(core::app::has_addons_c<addons::ECS> auto&)
        -> ecs::DependencyProvider;
};

}   // namespace extensions::scheduler::dependency_providers

template <core::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
extensions::scheduler::dependency_providers::tags::ecs::Query<Parameters_T...>::Query(
    core::ecs::Registry& registry
)
    : m_registry{ registry }
{}

template <core::ecs::query_parameter_c... Parameters_T>
    requires(sizeof...(Parameters_T) != 0)
template <typename F>
auto extensions::scheduler::dependency_providers::tags::ecs::Query<Parameters_T...>::
    for_each(F&& func) const -> F
{
    return core::ecs::query<Parameters_T...>(m_registry, std::forward<F>(func));
}

constexpr auto extensions::scheduler::dependency_providers::ECS::operator()(
    core::app::has_addons_c<addons::ECS> auto& app
) -> ecs::DependencyProvider
{
    return ecs::DependencyProvider{ app.registry };
}

extensions::scheduler::dependency_providers::ecs::DependencyProvider::DependencyProvider(
    core::ecs::Registry& registry
)
    : m_registry{ registry }
{}

template <util::meta::specialization_of_c<
    extensions::scheduler::dependency_providers::tags::ecs::Query> Query_T>
auto extensions::scheduler::dependency_providers::ecs::DependencyProvider::provide() const
    -> Query_T
{
    return Query_T{ m_registry };
}
