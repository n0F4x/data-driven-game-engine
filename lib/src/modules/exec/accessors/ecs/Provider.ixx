module;

#include <functional>
#include <type_traits>

export module ddge.modules.exec.accessors.ecs.Provider;

import ddge.modules.app;

import ddge.modules.ecs;
import ddge.modules.exec.accessors.ecs.Query;
import ddge.modules.exec.accessors.ecs.Registry;
import ddge.modules.exec.ProviderFor;
import ddge.modules.exec.ProviderOf;

import ddge.utility.meta.concepts.specialization_of;

namespace ddge::exec::accessors {

inline namespace ecs {

export class Provider {
public:
    template <ddge::app::has_addons_c<ddge::ecs::Addon> App_T>
    explicit Provider(App_T& app);

    template <std::same_as<accessors::ecs::Registry>>
    [[nodiscard]]
    auto provide() const -> accessors::ecs::Registry;

    template <util::meta::specialization_of_c<accessors::ecs::Query> Query_T>
    [[nodiscard]]
    auto provide() const -> Query_T;

private:
    std::reference_wrapper<ddge::ecs::Registry> m_registry;
};

}   // namespace ecs

}   // namespace ddge::exec::accessors

template <>
struct ddge::exec::ProviderFor<ddge::ecs::Addon>
    : std::type_identity<ddge::exec::accessors::ecs::Provider> {};

template <>
struct ddge::exec::ProviderOf<ddge::exec::accessors::ecs::Registry>
    : std::type_identity<ddge::exec::accessors::ecs::Provider> {};

template <typename... Filters_T>
struct ddge::exec::ProviderOf<ddge::exec::accessors::ecs::Query<Filters_T...>>
    : std::type_identity<ddge::exec::accessors::ecs::Provider> {};

template <ddge::app::has_addons_c<ddge::ecs::Addon> App_T>
ddge::exec::accessors::ecs::Provider::Provider(App_T& app) : m_registry{ app.registry }
{}

template <std::same_as<ddge::exec::accessors::ecs::Registry>>
auto ddge::exec::accessors::ecs::Provider::provide() const -> accessors::ecs::Registry
{
    return accessors::ecs::Registry{ m_registry };
}

template <ddge::util::meta::specialization_of_c<ddge::exec::accessors::ecs::Query> Query_T>
auto ddge::exec::accessors::ecs::Provider::provide() const -> Query_T
{
    return Query_T{ m_registry };
}
