module;

#include <functional>
#include <type_traits>

export module extensions.scheduler.argument_providers.resources.ResourceProvider;

import core.resources.ResourceManager;

import extensions.scheduler.accessors.resources;

import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.underlying;

namespace extensions::scheduler::argument_providers::resources {

export template <
    util::meta::specialization_of_c<core::resource::ResourceManager> ResourceManager_T>
class ResourceProvider {
public:
    constexpr explicit ResourceProvider(ResourceManager_T& resource_manager);

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

}   // namespace extensions::scheduler::argument_providers::resources

template <util::meta::specialization_of_c<core::resource::ResourceManager> ResourceManager_T>
constexpr extensions::scheduler::argument_providers::resources::
    ResourceProvider<ResourceManager_T>::ResourceProvider(
        ResourceManager_T& resource_manager
    )
    : m_resource_manager{ resource_manager }
{}

template <util::meta::specialization_of_c<core::resource::ResourceManager> ResourceManager_T>
template <typename Accessor_T>
    requires util::meta::specialization_of_c<
                 std::remove_cvref_t<Accessor_T>,
                 extensions::scheduler::accessors::resources::Ref>
          && (ResourceManager_T::template contains<std::remove_const_t<
                  util::meta::underlying_t<std::remove_cvref_t<Accessor_T>>>>())
constexpr auto extensions::scheduler::argument_providers::resources::
    ResourceProvider<ResourceManager_T>::provide() const
    -> std::remove_cvref_t<Accessor_T>
{
    return std::remove_cvref_t<Accessor_T>{
        m_resource_manager.get()
            .template get<std::remove_const_t<
                util::meta::underlying_t<std::remove_cvref_t<Accessor_T>>>>()
    };
}
