export module ddge.modules.resources.resource_c;

import ddge.modules.store.item_c;

namespace ddge::resources {

export template <typename T>
concept resource_c = ddge::store::item_c<T>;

}   // namespace ddge::resources
