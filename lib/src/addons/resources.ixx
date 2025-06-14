export module addons.Resources;

import core.resources;

namespace addons {

export struct ResourcesTag {};

export template <core::resources::resource_c... Resources_T>
struct Resources : ResourcesTag {
    core::resources::ResourceManager<Resources_T...> resource_manager;
};

}   // namespace addons
