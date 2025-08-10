module;

#include <functional>
#include <type_traits>

export module ddge.modules.scheduler.providers.ECSProvider;

import ddge.modules.app;

import ddge.modules.ecs;
import ddge.modules.scheduler.ProviderFor;

import ddge.modules.scheduler.accessors.ecs;
import ddge.modules.scheduler.ProviderOf;

import ddge.utility.meta.concepts.specialization_of;

namespace ddge::scheduler::providers {

export class ECSProvider {
public:
    template <ddge::app::has_addons_c<ddge::ecs::Addon> App_T>
    explicit ECSProvider(App_T& app);

    template <std::same_as<accessors::ecs::Registry>>
    [[nodiscard]]
    auto provide() const -> accessors::ecs::Registry;

    template <util::meta::specialization_of_c<accessors::ecs::Query> Query_T>
    [[nodiscard]]
    auto provide() const -> Query_T;

private:
    std::reference_wrapper<ddge::ecs::Registry> m_registry;
};

}   // namespace ddge::scheduler::providers

template <>
struct ddge::scheduler::ProviderOf<ddge::ecs::Addon>
    : std::type_identity<ddge::scheduler::providers::ECSProvider> {};

template <>
struct ddge::scheduler::ProviderFor<ddge::scheduler::accessors::ecs::Registry>
    : std::type_identity<ddge::scheduler::providers::ECSProvider> {};

template <typename... Parameters_T>
struct ddge::scheduler::ProviderFor<ddge::scheduler::accessors::ecs::Query<Parameters_T...>>
    : std::type_identity<ddge::scheduler::providers::ECSProvider> {};

template <ddge::app::has_addons_c<ddge::ecs::Addon> App_T>
ddge::scheduler::providers::ECSProvider::ECSProvider(App_T& app)
    : m_registry{ app.registry }
{}

template <std::same_as<ddge::scheduler::accessors::ecs::Registry>>
auto ddge::scheduler::providers::ECSProvider::provide() const -> accessors::ecs::Registry
{
    return accessors::ecs::Registry{ m_registry };
}

template <
    ddge::util::meta::specialization_of_c<ddge::scheduler::accessors::ecs::Query> Query_T>
auto ddge::scheduler::providers::ECSProvider::provide() const -> Query_T
{
    return Query_T{ m_registry };
}
