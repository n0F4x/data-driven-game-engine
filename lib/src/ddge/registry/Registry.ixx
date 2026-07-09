export module ddge.registry.Registry;

import ddge.registry.ErasedEntry;
import ddge.util.containers.GenericStack;

namespace ddge::registry {

export using Registry = util::BasicGenericStack<ErasedEntry>;

}   // namespace ddge::registry
