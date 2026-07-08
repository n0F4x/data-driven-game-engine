export module ddge.modules.resources.resource_c;

import ddge.util.containers.GenericStack;

namespace ddge::resources {

export template <typename T>
concept resource_c = ddge::util::generic_stack_item_c<T>;

}   // namespace ddge::resources
