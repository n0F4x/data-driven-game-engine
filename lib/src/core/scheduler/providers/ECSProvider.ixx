module;

#include <functional>
#include <type_traits>

export module core.scheduler.providers.ECSProvider;

import addons.ECS;

import app;

import core.ecs;
import core.scheduler.ProviderFor;

import core.scheduler.accessors.ecs;
import core.scheduler.ProviderOf;

import utility.meta.concepts.specialization_of;

namespace core::scheduler::providers {

export class ECSProvider {
public:
    template <app::has_addons_c<addons::ECS> App_T>
    explicit ECSProvider(App_T& app);

    template <std::same_as<accessors::ecs::Registry>>
    [[nodiscard]]
    auto provide() const -> accessors::ecs::Registry;

    template <util::meta::specialization_of_c<accessors::ecs::Query> Query_T>
    [[nodiscard]]
    auto provide() const -> Query_T;

private:
    std::reference_wrapper<core::ecs::Registry> m_registry;
};

}   // namespace core::scheduler::providers

template <>
struct core::scheduler::ProviderOf<addons::ECS>
    : std::type_identity<core::scheduler::providers::ECSProvider> {};

template <>
struct core::scheduler::ProviderFor<core::scheduler::accessors::ecs::Registry>
    : std::type_identity<core::scheduler::providers::ECSProvider> {};

template <typename... Parameters_T>
struct core::scheduler::
    ProviderFor<core::scheduler::accessors::ecs::Query<Parameters_T...>>
    : std::type_identity<core::scheduler::providers::ECSProvider> {};

template <app::has_addons_c<addons::ECS> App_T>
core::scheduler::providers::ECSProvider::ECSProvider(App_T& app)
    : m_registry{ app.registry }
{}

template <std::same_as<core::scheduler::accessors::ecs::Registry>>
auto core::scheduler::providers::ECSProvider::provide() const
    -> accessors::ecs::Registry
{
    return accessors::ecs::Registry{ m_registry };
}

template <
    util::meta::specialization_of_c<core::scheduler::accessors::ecs::Query> Query_T>
auto core::scheduler::providers::ECSProvider::provide() const -> Query_T
{
    return Query_T{ m_registry };
}
