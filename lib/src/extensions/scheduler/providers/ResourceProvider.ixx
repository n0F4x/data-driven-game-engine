module;

#include <format>
#include <functional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module extensions.scheduler.providers.ResourceProvider;

import addons.Resources;

import app;

import core.resources.ResourceManager;
import core.scheduler.ProviderFor;

import extensions.scheduler.accessors.resources;
import extensions.scheduler.ProviderOf;

import utility.contracts;
import utility.meta.concepts.specialization_of;
import utility.meta.reflection.name_of;

namespace extensions::scheduler::providers {

export class ResourceProvider {
public:
    template <app::has_addons_c<addons::Resources> App_T>
    constexpr explicit ResourceProvider(App_T& app);

    template <util::meta::specialization_of_c<accessors::resources::Resource> Resource_T>
    [[nodiscard]]
    constexpr auto provide() const -> Resource_T;

private:
    std::reference_wrapper<core::resources::ResourceManager> m_resource_manager_ref;
};

}   // namespace extensions::scheduler::providers

template <>
struct extensions::scheduler::ProviderOf<addons::Resources>
    : std::type_identity<extensions::scheduler::providers::ResourceProvider> {};

template <typename Resource_T>
struct core::scheduler::
    ProviderFor<extensions::scheduler::accessors::resources::Resource<Resource_T>>
    : std::type_identity<extensions::scheduler::providers::ResourceProvider> {};

template <app::has_addons_c<addons::Resources> App_T>
constexpr extensions::scheduler::providers::ResourceProvider::ResourceProvider(App_T& app)
    : m_resource_manager_ref{ app.resource_manager }
{}

template <util::meta::specialization_of_c<
    extensions::scheduler::accessors::resources::Resource> Resource_T>
constexpr auto extensions::scheduler::providers::ResourceProvider::provide() const
    -> Resource_T
{
    using Resource = std::remove_const_t<typename Resource_T::Underlying>;

    PRECOND(
        (m_resource_manager_ref.get().contains<Resource>()),
        std::format("Resource `{}` does not exist", util::meta::name_of<Resource>())
    );

    return Resource_T{ m_resource_manager_ref.get().at<Resource>() };
}
