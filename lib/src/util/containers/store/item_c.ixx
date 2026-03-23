export module ddge.util.containers.store.item_c;

import ddge.util.meta.concepts.naked;
import ddge.util.meta.concepts.storable;

namespace ddge::util::store {

export template <typename T>
concept item_c = util::meta::storable_c<T> && util::meta::naked_c<T>;

}   // namespace ddge::util::store
