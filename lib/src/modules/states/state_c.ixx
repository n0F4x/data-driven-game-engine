module;

#include <optional>

export module ddge.modules.states.state_c;

import ddge.utility.containers.store.item_c;

namespace ddge::states {

export template <typename T>
concept state_c = util::store::item_c<std::optional<T>>;

}   // namespace ddge::states
