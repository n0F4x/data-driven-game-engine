module;

#include <utility>

export module ddge.resources.ResourceManager;

import ddge.resources.resource_c;
import ddge.util.containers.GenericStack;

import ddge.util.containers.OptionalRef;
import ddge.util.meta.type_traits.const_like;
import ddge.util.meta.type_traits.forward_like;

namespace ddge::resources {

export class ResourceManager {
public:
    ResourceManager() = default;
    explicit ResourceManager(util::GenericStack&& resources);

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
    util::GenericStack m_resources;
};

}   // namespace ddge::resources

ddge::resources::ResourceManager::ResourceManager(util::GenericStack&& resources)
    : m_resources{ std::move(resources) }
{}

template <ddge::resources::resource_c Resource_T, typename Self_T>
auto ddge::resources::ResourceManager::find(this Self_T& self) noexcept
    -> util::OptionalRef<util::meta::const_like_t<Resource_T, Self_T>>
{
    return self.m_resources.template find<Resource_T>();
}

template <ddge::resources::resource_c Resource_T, typename Self_T>
auto ddge::resources::ResourceManager::at(this Self_T&& self)
    -> util::meta::forward_like_t<Resource_T, Self_T>
{
    return std::forward_like<Self_T>(self.m_resources).template at<Resource_T>();
}

template <ddge::resources::resource_c Resource_T>
auto ddge::resources::ResourceManager::contains() const noexcept -> bool
{
    return m_resources.contains<Resource_T>();
}
