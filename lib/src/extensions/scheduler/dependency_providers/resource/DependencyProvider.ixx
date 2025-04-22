module;

#include <functional>

export module extensions.scheduler.dependency_providers.resource.DependencyProvider;

import core.resource.ResourceManager;

import extensions.scheduler.accessors.resource;

import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.underlying;

namespace extensions::scheduler::dependency_providers::resource {

export template <
    util::meta::specialization_of_c<core::resource::ResourceManager> ResourceManager_T>
class DependencyProvider {
public:
    constexpr explicit DependencyProvider(ResourceManager_T& resource_manager);

    template <util::meta::specialization_of_c<accessors::resource::Res> ResourceTag_T>
    [[nodiscard]]
    constexpr auto provide() const -> ResourceTag_T;

private:
    std::reference_wrapper<ResourceManager_T> m_resource_manager;
};

}   // namespace extensions::scheduler::dependency_providers::resource

template <util::meta::specialization_of_c<core::resource::ResourceManager> ResourceManager_T>
constexpr extensions::scheduler::dependency_providers::resource::
    DependencyProvider<ResourceManager_T>::DependencyProvider(
        ResourceManager_T& resource_manager
    )
    : m_resource_manager{ resource_manager }
{}

template <util::meta::specialization_of_c<core::resource::ResourceManager> ResourceManager_T>
template <util::meta::specialization_of_c<extensions::scheduler::accessors::resource::Res>
              ResourceTag_T>
constexpr auto extensions::scheduler::dependency_providers::resource::
    DependencyProvider<ResourceManager_T>::provide() const -> ResourceTag_T
{
    return ResourceTag_T{
        m_resource_manager.get()
            .template get<std::remove_const_t<util::meta::underlying_t<ResourceTag_T>>>()
    };
}
