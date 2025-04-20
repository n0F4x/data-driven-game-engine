module;

#include <utility>

export module addons.ResourceManager;

import core.resource.resource_c;
import core.resource.ResourceManager;

namespace addons {

export template <typename T>
concept resource_c = ::core::resource::resource_c<T>;

export struct ResourceManagerTag {};

export template <typename... Resources_T>
struct ResourceManager : ResourceManagerTag {
    template <typename... Args>
    constexpr explicit ResourceManager(std::in_place_t, Args&&... args);

    ::core::resource::ResourceManager<Resources_T...> resource_manager;
};

template <typename... Resources_T>
template <typename... Args>
constexpr ResourceManager<Resources_T...>::ResourceManager(std::in_place_t, Args&&... args)
    : resource_manager{ std::forward<Args>(args)... }
{}

}   // namespace addons
