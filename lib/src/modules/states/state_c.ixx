export module ddge.modules.states.state_c;

import ddge.modules.store.item_c;

namespace ddge::states {

export template <typename T>
concept state_c = store::item_c<T>;

}   // namespace ddge::states
