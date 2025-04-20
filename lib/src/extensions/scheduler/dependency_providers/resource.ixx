module;

#include <functional>
#include <type_traits>

export module extensions.scheduler.dependency_providers.resource;

import addons.ResourceManager;

import core.app.has_addons_c;
import core.resource.resource_c;
import core.resource.ResourceManager;

import utility.containers.OptionalRef;
import utility.meta.concepts.specialization_of;
import utility.meta.type_traits.underlying;

namespace extensions::scheduler::dependency_providers {

inline namespace tags {

export template <typename Resource_T>
    requires core::resource::resource_c<std::remove_const_t<Resource_T>>
class Res {
public:
    constexpr explicit Res(Resource_T& ref);

    [[nodiscard]]
    auto get() const -> Resource_T&;

    [[nodiscard]]
    constexpr auto operator->() const -> Resource_T*;

private:
    std::reference_wrapper<Resource_T> m_ref;
};

}   // namespace tags

namespace resource {

template <util::meta::specialization_of_c<core::resource::ResourceManager> ResourceManager_T>
class DependencyProvider {
public:
    constexpr explicit DependencyProvider(ResourceManager_T& resource_manager);

    template <util::meta::specialization_of_c<tags::Res> ResourceTag_T>
    [[nodiscard]]
    constexpr auto provide() -> ResourceTag_T;

private:
    std::reference_wrapper<ResourceManager_T> m_resource_manager;
};

}   // namespace resource

export struct ResourceManager {
    [[nodiscard]]
    constexpr static auto operator()(
        core::app::has_addons_c<addons::ResourceManagerTag> auto& app
    );
};

}   // namespace extensions::scheduler::dependency_providers

template <typename Resource_T>
    requires core::resource::resource_c<std::remove_const_t<Resource_T>>
constexpr extensions::scheduler::dependency_providers::tags::Res<Resource_T>::Res(
    Resource_T& ref
)
    : m_ref{ ref }
{}

template <typename Resource_T>
    requires core::resource::resource_c<std::remove_const_t<Resource_T>>
auto extensions::scheduler::dependency_providers::tags::Res<Resource_T>::get() const
    -> Resource_T&
{
    return m_ref.get();
}

template <typename Resource_T>
    requires core::resource::resource_c<std::remove_const_t<Resource_T>>
constexpr auto
    extensions::scheduler::dependency_providers::tags::Res<Resource_T>::operator->() const
    -> Resource_T*
{
    return std::addressof(m_ref.get());
}

template <util::meta::specialization_of_c<core::resource::ResourceManager> ResourceManager_T>
constexpr extensions::scheduler::dependency_providers::resource::
    DependencyProvider<ResourceManager_T>::DependencyProvider(
        ResourceManager_T& resource_manager
    )
    : m_resource_manager{ resource_manager }
{}

template <util::meta::specialization_of_c<core::resource::ResourceManager> ResourceManager_T>
template <util::meta::specialization_of_c<extensions::scheduler::dependency_providers::Res>
              ResourceTag_T>
constexpr auto extensions::scheduler::dependency_providers::resource::
    DependencyProvider<ResourceManager_T>::provide() -> ResourceTag_T
{
    return ResourceTag_T{
        m_resource_manager.get()
            .template get<std::remove_const_t<util::meta::underlying_t<ResourceTag_T>>>()
    };
}

constexpr auto extensions::scheduler::dependency_providers::ResourceManager::operator()(
    core::app::has_addons_c<addons::ResourceManagerTag> auto& app
)
{
    return resource::DependencyProvider{ app.resource_manager };
}
