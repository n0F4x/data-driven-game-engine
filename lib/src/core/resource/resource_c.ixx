export module core.resource.resource_c;

import utility.meta.concepts.decayed;

namespace core::resource {

export template <typename T>
concept resource_c = ::util::meta::decayed_c<T>;

}   // namespace core::resource
