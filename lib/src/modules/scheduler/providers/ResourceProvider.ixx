module;

#include <format>
#include <functional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module ddge.modules.scheduler.providers.ResourceProvider;

import ddge.modules.app;
import ddge.modules.resources;
import ddge.modules.scheduler.ProviderFor;

import ddge.modules.scheduler.accessors.resources;
import ddge.modules.scheduler.ProviderOf;

import ddge.utility.contracts;
import ddge.utility.meta.concepts.specialization_of;
import ddge.utility.meta.reflection.name_of;

namespace ddge::scheduler::providers {

export class ResourceProvider {
public:
    template <ddge::app::has_addons_c<resources::Addon> App_T>
    constexpr explicit ResourceProvider(App_T& app);

    template <util::meta::specialization_of_c<accessors::resources::Resource> Resource_T>
    [[nodiscard]]
    constexpr auto provide() const -> Resource_T;

private:
    std::reference_wrapper<ddge::resources::ResourceManager> m_resource_manager_ref;
};

}   // namespace ddge::scheduler::providers

template <>
struct ddge::scheduler::ProviderOf<ddge::resources::Addon>
    : std::type_identity<ddge::scheduler::providers::ResourceProvider> {};

template <typename Resource_T>
struct ddge::scheduler::
    ProviderFor<ddge::scheduler::accessors::resources::Resource<Resource_T>>
    : std::type_identity<ddge::scheduler::providers::ResourceProvider> {};

template <ddge::app::has_addons_c<ddge::resources::Addon> App_T>
constexpr ddge::scheduler::providers::ResourceProvider::ResourceProvider(App_T& app)
    : m_resource_manager_ref{ app.resource_manager }
{}

template <ddge::util::meta::specialization_of_c<
    ddge::scheduler::accessors::resources::Resource> Resource_T>
constexpr auto ddge::scheduler::providers::ResourceProvider::provide() const -> Resource_T
{
    using Resource = std::remove_const_t<typename Resource_T::Underlying>;

    PRECOND(
        (m_resource_manager_ref.get().contains<Resource>()),
        std::format("Resource `{}` does not exist", util::meta::name_of<Resource>())
    );

    return Resource_T{ m_resource_manager_ref.get().at<Resource>() };
}
