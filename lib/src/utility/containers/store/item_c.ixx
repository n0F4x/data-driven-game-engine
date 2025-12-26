export module ddge.utility.containers.store.item_c;

import ddge.utility.meta.concepts.naked;
import ddge.utility.meta.concepts.storable;

namespace ddge::util::store {

export template <typename T>
concept item_c = util::meta::storable_c<T> && util::meta::naked_c<T>;

}   // namespace ddge::util::store
