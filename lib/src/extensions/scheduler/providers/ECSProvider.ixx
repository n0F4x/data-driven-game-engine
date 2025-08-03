module;

#include <functional>
#include <type_traits>

export module extensions.scheduler.providers.ECSProvider;

import addons.ECS;

import app;

import core.ecs;
import core.scheduler.ProviderFor;

import extensions.scheduler.accessors.ecs;
import extensions.scheduler.ProviderOf;

import utility.meta.concepts.specialization_of;

namespace extensions::scheduler::providers {

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

}   // namespace extensions::scheduler::providers

template <>
struct extensions::scheduler::ProviderOf<addons::ECS>
    : std::type_identity<extensions::scheduler::providers::ECSProvider> {};

template <>
struct core::scheduler::ProviderFor<extensions::scheduler::accessors::ecs::Registry>
    : std::type_identity<extensions::scheduler::providers::ECSProvider> {};

template <typename... Parameters_T>
struct core::scheduler::
    ProviderFor<extensions::scheduler::accessors::ecs::Query<Parameters_T...>>
    : std::type_identity<extensions::scheduler::providers::ECSProvider> {};

template <app::has_addons_c<addons::ECS> App_T>
extensions::scheduler::providers::ECSProvider::ECSProvider(App_T& app)
    : m_registry{ app.registry }
{}

template <std::same_as<extensions::scheduler::accessors::ecs::Registry>>
auto extensions::scheduler::providers::ECSProvider::provide() const
    -> accessors::ecs::Registry
{
    return accessors::ecs::Registry{ m_registry };
}

template <
    util::meta::specialization_of_c<extensions::scheduler::accessors::ecs::Query> Query_T>
auto extensions::scheduler::providers::ECSProvider::provide() const -> Query_T
{
    return Query_T{ m_registry };
}
