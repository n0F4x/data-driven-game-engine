module;

#include <functional>
#include <utility>

export module core.resources.ResourceManager;

import core.resources.resource_c;
import core.store.Store;

import utility.containers.OptionalRef;
import utility.meta.concepts.functional.unambiguously_invocable;
import utility.meta.type_traits.const_like;
import utility.meta.type_traits.forward_like;
import utility.meta.type_traits.functional.arguments_of;
import utility.meta.type_traits.functional.result_of;
import utility.TypeList;

namespace core::resources {

export class ResourceManager {
public:
    template <util::meta::unambiguously_invocable_c... Factories_T>
    explicit ResourceManager(Factories_T&&... factories);

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

template <typename Factory_T>
[[nodiscard]]
auto produce_resource(Factory_T&& factory, core::store::Store& store)
    -> util::meta::result_of_t<Factory_T>
{
    return [&factory,
            &store]<typename... Dependencies_T>(util::TypeList<Dependencies_T...>) {
        return std::invoke(
            std::forward<Factory_T>(factory),
            store.at<std::remove_cvref_t<Dependencies_T>>()...
        );
    }(util::meta::arguments_of_t<Factory_T>{});
}

template <util::meta::unambiguously_invocable_c... Factories_T>
core::resources::ResourceManager::ResourceManager(Factories_T&&... factories)
{
    (m_store.emplace<util::meta::result_of_t<Factories_T>>(
         ::produce_resource(std::forward<Factories_T>(factories), m_store)
     ),
     ...);
}

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
