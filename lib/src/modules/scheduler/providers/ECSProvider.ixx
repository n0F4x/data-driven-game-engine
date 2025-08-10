module;

#include <functional>
#include <type_traits>

export module modules.scheduler.providers.ECSProvider;

import modules.app;

import modules.ecs;
import modules.scheduler.ProviderFor;

import modules.scheduler.accessors.ecs;
import modules.scheduler.ProviderOf;

import utility.meta.concepts.specialization_of;

namespace modules::scheduler::providers {

export class ECSProvider {
public:
    template <modules::app::has_addons_c<modules::ecs::Addon> App_T>
    explicit ECSProvider(App_T& app);

    template <std::same_as<accessors::ecs::Registry>>
    [[nodiscard]]
    auto provide() const -> accessors::ecs::Registry;

    template <util::meta::specialization_of_c<accessors::ecs::Query> Query_T>
    [[nodiscard]]
    auto provide() const -> Query_T;

private:
    std::reference_wrapper<modules::ecs::Registry> m_registry;
};

}   // namespace modules::scheduler::providers

template <>
struct modules::scheduler::ProviderOf<modules::ecs::Addon>
    : std::type_identity<modules::scheduler::providers::ECSProvider> {};

template <>
struct modules::scheduler::ProviderFor<modules::scheduler::accessors::ecs::Registry>
    : std::type_identity<modules::scheduler::providers::ECSProvider> {};

template <typename... Parameters_T>
struct modules::scheduler::
    ProviderFor<modules::scheduler::accessors::ecs::Query<Parameters_T...>>
    : std::type_identity<modules::scheduler::providers::ECSProvider> {};

template <modules::app::has_addons_c<modules::ecs::Addon> App_T>
modules::scheduler::providers::ECSProvider::ECSProvider(App_T& app)
    : m_registry{ app.registry }
{}

template <std::same_as<modules::scheduler::accessors::ecs::Registry>>
auto modules::scheduler::providers::ECSProvider::provide() const
    -> accessors::ecs::Registry
{
    return accessors::ecs::Registry{ m_registry };
}

template <
    util::meta::specialization_of_c<modules::scheduler::accessors::ecs::Query> Query_T>
auto modules::scheduler::providers::ECSProvider::provide() const -> Query_T
{
    return Query_T{ m_registry };
}
