module;

#include <functional>
#include <type_traits>

export module ddge.modules.scheduler.accessors.ecs.Provider;

import ddge.modules.app;

import ddge.modules.ecs;
import ddge.modules.scheduler.accessors.ecs.Query;
import ddge.modules.scheduler.accessors.ecs.Registry;
import ddge.modules.scheduler.ProviderFor;
import ddge.modules.scheduler.ProviderOf;

import ddge.utility.meta.concepts.specialization_of;

namespace ddge::scheduler::accessors {

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

}   // namespace ddge::scheduler::accessors

template <>
struct ddge::scheduler::ProviderFor<ddge::ecs::Addon>
    : std::type_identity<ddge::scheduler::accessors::ecs::Provider> {};

template <>
struct ddge::scheduler::ProviderOf<ddge::scheduler::accessors::ecs::Registry>
    : std::type_identity<ddge::scheduler::accessors::ecs::Provider> {};

template <typename... Filters_T>
struct ddge::scheduler::ProviderOf<ddge::scheduler::accessors::ecs::Query<Filters_T...>>
    : std::type_identity<ddge::scheduler::accessors::ecs::Provider> {};

template <ddge::app::has_addons_c<ddge::ecs::Addon> App_T>
ddge::scheduler::accessors::ecs::Provider::Provider(App_T& app)
    : m_registry{ app.registry }
{}

template <std::same_as<ddge::scheduler::accessors::ecs::Registry>>
auto ddge::scheduler::accessors::ecs::Provider::provide() const
    -> accessors::ecs::Registry
{
    return accessors::ecs::Registry{ m_registry };
}

template <
    ddge::util::meta::specialization_of_c<ddge::scheduler::accessors::ecs::Query> Query_T>
auto ddge::scheduler::accessors::ecs::Provider::provide() const -> Query_T
{
    return Query_T{ m_registry };
}
