export module core.resources.resource_c;

import utility.meta.concepts.naked;
import utility.meta.concepts.storable;

namespace core::resources {

export template <typename T>
concept resource_c = ::util::meta::storable_c<T> && util::meta::naked_c<T>;

}   // namespace core::resources
