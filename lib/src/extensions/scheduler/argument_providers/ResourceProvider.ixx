module;

#include <functional>
#include <type_traits>

export module extensions.scheduler.argument_providers.ResourceProvider;

import app;

import core.resources.ResourceManager;

import extensions.scheduler.accessors.resources;

import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.underlying;

namespace extensions::scheduler::argument_providers {

export template <
    util::meta::specialization_of_c<core::resources::ResourceManager> ResourceManager_T,
    typename ResourcesAddon_T>
class ResourceProvider {
public:
    template <app::has_addons_c<ResourcesAddon_T> App_T>
    constexpr explicit ResourceProvider(App_T& app);

    template <typename Accessor_T>
        requires util::meta::specialization_of_c<
                     std::remove_cvref_t<Accessor_T>,
                     accessors::resources::Ref>
              && (ResourceManager_T::template contains<std::remove_const_t<
                      util::meta::underlying_t<std::remove_cvref_t<Accessor_T>>>>())
    [[nodiscard]] constexpr auto provide() const -> std::remove_cvref_t<Accessor_T>;

private:
    std::reference_wrapper<ResourceManager_T> m_resource_manager;
};

}   // namespace extensions::scheduler::argument_providers

template <
    util::meta::specialization_of_c<core::resources::ResourceManager> ResourceManager_T,
    typename ResourcesAddon_T>
template <app::has_addons_c<ResourcesAddon_T> App_T>
constexpr extensions::scheduler::argument_providers::
    ResourceProvider<ResourceManager_T, ResourcesAddon_T>::ResourceProvider(App_T& app)
    : m_resource_manager{ app.resource_manager }
{}

template <
    util::meta::specialization_of_c<core::resources::ResourceManager> ResourceManager_T,
    typename ResourcesAddon_T>
template <typename Accessor_T>
    requires util::meta::specialization_of_c<
                 std::remove_cvref_t<Accessor_T>,
                 extensions::scheduler::accessors::resources::Ref>
          && (ResourceManager_T::template contains<std::remove_const_t<
                  util::meta::underlying_t<std::remove_cvref_t<Accessor_T>>>>())
constexpr auto extensions::scheduler::argument_providers::
    ResourceProvider<ResourceManager_T, ResourcesAddon_T>::provide() const
    -> std::remove_cvref_t<Accessor_T>
{
    return std::remove_cvref_t<Accessor_T>{
        m_resource_manager.get()
            .template get<std::remove_const_t<
                util::meta::underlying_t<std::remove_cvref_t<Accessor_T>>>>()
    };
}
