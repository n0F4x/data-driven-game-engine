module;

#include <concepts>

export module core.store.item_c;

import utility.meta.concepts.naked;
import utility.meta.concepts.storable;

namespace core::store {

export template <typename T>
concept item_c = util::meta::storable_c<T> && util::meta::naked_c<T> && std::movable<T>;

}   // namespace core::store
