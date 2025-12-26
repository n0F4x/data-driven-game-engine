module;

#include <utility>

export module ddge.modules.resources.ResourceManager;

import ddge.modules.resources.resource_c;
import ddge.utility.containers.store.Store;

import ddge.utility.containers.OptionalRef;
import ddge.utility.meta.type_traits.const_like;
import ddge.utility.meta.type_traits.forward_like;

namespace ddge::resources {

export class ResourceManager {
public:
    ResourceManager() = default;
    explicit ResourceManager(util::store::Store&& store);

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
    ddge::util::store::Store m_store;
};

}   // namespace ddge::resources

ddge::resources::ResourceManager::ResourceManager(util::store::Store&& store)
    : m_store{ std::move(store) }
{}

template <ddge::resources::resource_c Resource_T, typename Self_T>
auto ddge::resources::ResourceManager::find(this Self_T& self) noexcept
    -> util::OptionalRef<util::meta::const_like_t<Resource_T, Self_T>>
{
    return self.m_store.template find<Resource_T>();
}

template <ddge::resources::resource_c Resource_T, typename Self_T>
auto ddge::resources::ResourceManager::at(this Self_T&& self)
    -> util::meta::forward_like_t<Resource_T, Self_T>
{
    return std::forward_like<Self_T>(self.m_store).template at<Resource_T>();
}

template <ddge::resources::resource_c Resource_T>
auto ddge::resources::ResourceManager::contains() const noexcept -> bool
{
    return m_store.contains<Resource_T>();
}
