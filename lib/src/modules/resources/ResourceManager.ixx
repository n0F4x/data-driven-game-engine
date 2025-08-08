module;

#include <utility>

export module modules.resources.ResourceManager;

import modules.resources.resource_c;
import modules.store.Store;

import utility.containers.OptionalRef;
import utility.meta.type_traits.const_like;
import utility.meta.type_traits.forward_like;

namespace modules::resources {

export class ResourceManager {
public:
    ResourceManager() = default;
    explicit ResourceManager(store::Store&& store);

    template <resource_c Resource_T, typename Self_T>
    [[nodiscard]]
    auto find(this Self_T&) noexcept
        -> util::OptionalRef<util::meta::const_like_t<Resource_T, Self_T>>;

    template <resource_c Resource_T, typename Self_T>
    [[nodiscard]]
    auto at(this Self_T&&) -> util::meta::forward_like_t<Resource_T, Self_T>;

    template <resource_c Resource_T>
    [[nodiscard]]
    auto contains() const noexcept -> bool;

private:
    modules::store::Store m_store;
};

}   // namespace modules::resources

modules::resources::ResourceManager::ResourceManager(store::Store&& store)
    : m_store{ std::move(store) }
{}

template <modules::resources::resource_c Resource_T, typename Self_T>
auto modules::resources::ResourceManager::find(this Self_T& self) noexcept
    -> util::OptionalRef<util::meta::const_like_t<Resource_T, Self_T>>
{
    return self.m_store.template find<Resource_T>();
}

template <modules::resources::resource_c Resource_T, typename Self_T>
auto modules::resources::ResourceManager::at(this Self_T&& self)
    -> util::meta::forward_like_t<Resource_T, Self_T>
{
    return std::forward_like<Self_T>(self.m_store).template at<Resource_T>();
}

template <modules::resources::resource_c Resource_T>
auto modules::resources::ResourceManager::contains() const noexcept -> bool
{
    return m_store.contains<Resource_T>();
}
