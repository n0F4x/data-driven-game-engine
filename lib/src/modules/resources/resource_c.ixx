export module modules.resources.resource_c;

import modules.store.item_c;

namespace modules::resources {

export template <typename T>
concept resource_c = modules::store::item_c<T>;

}   // namespace modules::resources
