export module ddge.registry.ErasedEntry;

import ddge.registry.entry_c;
import ddge.util.containers.MoveOnlyAny;

namespace ddge::registry {

template <typename T>
struct EntryConcept {
    constexpr static bool value{ entry_c<T> };
};

export using ErasedEntry =
    util::BasicMoveOnlyAny<0, util::default_any_alignment(), EntryConcept>;

}   // namespace ddge::registry
