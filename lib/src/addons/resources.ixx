module;

#include <utility>

export module addons.Resources;

import core.resources;

namespace addons {

export struct ResourcesTag {};

export template <typename... Resources_T>
struct Resources : ResourcesTag {
    template <typename... Args>
    constexpr explicit Resources(std::in_place_t, Args&&... args);

    core::resource::ResourceManager<Resources_T...> resource_manager;
};

template <typename... Resources_T>
template <typename... Args>
constexpr Resources<Resources_T...>::Resources(std::in_place_t, Args&&... args)
    : resource_manager{ std::forward<Args>(args)... }
{}

}   // namespace addons
