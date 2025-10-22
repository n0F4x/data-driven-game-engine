module;

#include <functional>
#include <type_traits>

export module ddge.modules.exec.providers.ECSProvider;

import ddge.modules.app;

import ddge.modules.ecs;
import ddge.modules.exec.ProviderFor;

import ddge.modules.exec.accessors.ecs;
import ddge.modules.exec.ProviderOf;

import ddge.utility.meta.concepts.specialization_of;

namespace ddge::exec::providers {

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

}   // namespace ddge::exec::providers

template <>
struct ddge::exec::ProviderOf<ddge::ecs::Addon>
    : std::type_identity<ddge::exec::providers::ECSProvider> {};

template <>
struct ddge::exec::ProviderFor<ddge::exec::accessors::ecs::Registry>
    : std::type_identity<ddge::exec::providers::ECSProvider> {};

template <typename... Filters_T>
struct ddge::exec::ProviderFor<ddge::exec::accessors::ecs::Query<Filters_T...>>
    : std::type_identity<ddge::exec::providers::ECSProvider> {};

template <ddge::app::has_addons_c<ddge::ecs::Addon> App_T>
ddge::exec::providers::ECSProvider::ECSProvider(App_T& app)
    : m_registry{ app.registry }
{}

template <std::same_as<ddge::exec::accessors::ecs::Registry>>
auto ddge::exec::providers::ECSProvider::provide() const -> accessors::ecs::Registry
{
    return accessors::ecs::Registry{ m_registry };
}

template <
    ddge::util::meta::specialization_of_c<ddge::exec::accessors::ecs::Query> Query_T>
auto ddge::exec::providers::ECSProvider::provide() const -> Query_T
{
    return Query_T{ m_registry };
}
