module;

#include <utility>

export module addons.ResourceManager;

import core.resources.resource_c;
import core.resources.ResourceManager;

namespace addons {

export struct ResourceManagerTag {};

export template <typename... Resources_T>
struct ResourceManager : ResourceManagerTag {
    template <typename... Args>
    constexpr explicit ResourceManager(std::in_place_t, Args&&... args);

    core::resource::ResourceManager<Resources_T...> resource_manager;
};

template <typename... Resources_T>
template <typename... Args>
constexpr ResourceManager<Resources_T...>::ResourceManager(std::in_place_t, Args&&... args)
    : resource_manager{ std::forward<Args>(args)... }
{}

}   // namespace addons
