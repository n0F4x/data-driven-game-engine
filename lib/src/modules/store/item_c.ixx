module;

#include <concepts>

export module modules.store.item_c;

import utility.meta.concepts.naked;
import utility.meta.concepts.storable;

namespace modules::store {

export template <typename T>
concept item_c = util::meta::storable_c<T> && util::meta::naked_c<T> && std::movable<T>;

}   // namespace modules::store
