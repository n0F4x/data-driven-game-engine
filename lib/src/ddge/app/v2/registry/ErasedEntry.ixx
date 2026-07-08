export module ddge.app.v2.registry.ErasedEntry;

import ddge.app.v2.registry.entry_c;
import ddge.util.containers.MoveOnlyAny;

namespace ddge::app::v2 {

template <typename T>
struct EntryConcept {
    constexpr static bool value{ entry_c<T> };
};

export using ErasedEntry =
    util::BasicMoveOnlyAny<0, util::default_any_alignment(), EntryConcept>;

}   // namespace ddge::app::v2
