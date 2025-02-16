export module addons.ResourceManager;

import core.store.Store;

namespace addons {

export template<typename T>
concept resource_c = core::store::storable_c<T>;

export struct ResourceManager {
    core::store::Store resources;
};

}   // namespace addons
