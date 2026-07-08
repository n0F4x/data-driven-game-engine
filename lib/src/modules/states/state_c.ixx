module;

#include <optional>

export module ddge.modules.states.state_c;

import ddge.util.containers.GenericStack;

namespace ddge::states {

export template <typename T>
concept state_c = util::generic_stack_item_c<std::optional<T>>;

}   // namespace ddge::states
