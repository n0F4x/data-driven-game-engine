export module core.states.state_c;

import core.store.item_c;

namespace core::states {

export template <typename T>
concept state_c = store::item_c<T>;

}   // namespace core::states
