module;

#include <tuple>

export module addons.ResourceManager;

import core.store.Store;

namespace addons {

export template <typename T>
concept resource_c = core::store::storable_c<T>;

export template <typename... Resources_T>
struct BasicResourceManager {
    std::tuple<Resources_T...> resources;
};

export using ResourceManager = BasicResourceManager<>;

export struct ResourceManagerTag {};

}   // namespace addons
