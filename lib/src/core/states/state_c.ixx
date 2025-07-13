export module core.states.state_c;

import utility.meta.concepts.naked;
import utility.meta.concepts.storable;

namespace core::states {

export template <typename T>
concept state_c = util::meta::storable_c<T> && util::meta::naked_c<T>;

}   // namespace core::states
