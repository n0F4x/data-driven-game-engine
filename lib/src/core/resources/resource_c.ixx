export module core.resources.resource_c;

import utility.meta.concepts.decayed;

namespace core::resources {

export template <typename T>
concept resource_c = ::util::meta::decayed_c<T>;

}   // namespace core::resource
