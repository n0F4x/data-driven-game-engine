module;

#include <concepts>

export module ddge.modules.store.item_c;

import ddge.utility.meta.concepts.naked;
import ddge.utility.meta.concepts.storable;

namespace ddge::store {

export template <typename T>
concept item_c = util::meta::storable_c<T> && util::meta::naked_c<T> && std::movable<T>;

}   // namespace ddge::store
