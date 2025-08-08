export module modules.states.state_c;

import modules.store.item_c;

namespace modules::states {

export template <typename T>
concept state_c = store::item_c<T>;

}   // namespace modules::states
