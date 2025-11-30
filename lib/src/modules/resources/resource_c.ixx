export module ddge.modules.resources.resource_c;

import ddge.utility.containers.store.item_c;

namespace ddge::resources {

export template <typename T>
concept resource_c = ddge::utility::store::item_c<T>;

}   // namespace ddge::resources
