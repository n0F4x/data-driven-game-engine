module;

#include <tuple>

export module addons.ResourceManager;

import core.store.Store;

namespace addons {

export template <typename T>
concept resource_c = core::store::storable_c<T>;

export struct ResourceManager {
    core::store::Store resources;
};

}   // namespace addons

namespace addons::v2 {

export struct ResourceManagerTag{};

export template <typename... Resources_T>
struct ResourceManager {
    std::tuple<Resources_T...> resources;
};

}   // namespace addons::v2
