export module ddge.modules.app.v2.registry.Registry;

import ddge.modules.app.v2.registry.ErasedEntry;
import ddge.util.containers.GenericStack;

namespace ddge::app::v2 {

export using Registry = util::BasicGenericStack<ErasedEntry>;

}   // namespace ddge::app::v2
