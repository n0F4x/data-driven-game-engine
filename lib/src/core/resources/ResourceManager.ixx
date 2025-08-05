module;

#include <utility>

export module core.resources.ResourceManager;

import core.resources.resource_c;
import core.store.Store;

import utility.containers.OptionalRef;
import utility.meta.type_traits.const_like;
import utility.meta.type_traits.forward_like;

namespace core::resources {

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
    core::store::Store m_store;
};

}   // namespace core::resources

core::resources::ResourceManager::ResourceManager(store::Store&& store)
    : m_store{ std::move(store) }
{}

template <core::resources::resource_c Resource_T, typename Self_T>
auto core::resources::ResourceManager::find(this Self_T& self) noexcept
    -> util::OptionalRef<util::meta::const_like_t<Resource_T, Self_T>>
{
    return self.m_store.template find<Resource_T>();
}

template <core::resources::resource_c Resource_T, typename Self_T>
auto core::resources::ResourceManager::at(this Self_T&& self)
    -> util::meta::forward_like_t<Resource_T, Self_T>
{
    return std::forward_like<Self_T>(self.m_store).template at<Resource_T>();
}

template <core::resources::resource_c Resource_T>
auto core::resources::ResourceManager::contains() const noexcept -> bool
{
    return m_store.contains<Resource_T>();
}
