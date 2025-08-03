export module core.resources.resource_c;

import core.store.item_c;

namespace core::resources {

export template <typename T>
concept resource_c = core::store::item_c<T>;

}   // namespace core::resources
