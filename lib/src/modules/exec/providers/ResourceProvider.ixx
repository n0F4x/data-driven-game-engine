module;

#include <format>
#include <functional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module ddge.modules.exec.providers.ResourceProvider;

import ddge.modules.app;
import ddge.modules.resources;
import ddge.modules.exec.ProviderFor;

import ddge.modules.exec.accessors.resources;
import ddge.modules.exec.ProviderOf;

import ddge.utility.contracts;
import ddge.utility.meta.concepts.specialization_of;
import ddge.utility.meta.reflection.name_of;

namespace ddge::exec::providers {

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

}   // namespace ddge::exec::providers

template <>
struct ddge::exec::ProviderOf<ddge::resources::Addon>
    : std::type_identity<ddge::exec::providers::ResourceProvider> {};

template <typename Resource_T>
struct ddge::exec::
    ProviderFor<ddge::exec::accessors::resources::Resource<Resource_T>>
    : std::type_identity<ddge::exec::providers::ResourceProvider> {};

template <ddge::app::has_addons_c<ddge::resources::Addon> App_T>
constexpr ddge::exec::providers::ResourceProvider::ResourceProvider(App_T& app)
    : m_resource_manager_ref{ app.resource_manager }
{}

template <ddge::util::meta::specialization_of_c<
    ddge::exec::accessors::resources::Resource> Resource_T>
constexpr auto ddge::exec::providers::ResourceProvider::provide() const -> Resource_T
{
    using Resource = std::remove_const_t<typename Resource_T::Underlying>;

    PRECOND(
        (m_resource_manager_ref.get().contains<Resource>()),
        std::format("Resource `{}` does not exist", util::meta::name_of<Resource>())
    );

    return Resource_T{ m_resource_manager_ref.get().at<Resource>() };
}
