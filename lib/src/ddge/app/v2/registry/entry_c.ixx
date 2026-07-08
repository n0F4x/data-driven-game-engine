module;

#include <concepts>
#include <type_traits>

export module ddge.app.v2.registry.entry_c;

import ddge.app.v2.registry.BuildableEntryBase;
import ddge.app.v2.registry.EntryBase;
import ddge.app.v2.registry.EntryBuilderBase;
import ddge.util.containers.MoveOnlyAny;

namespace ddge::app::v2 {

export template <typename T>
concept entry_c = util::storable_in_any_c<T, util::BasicMoveOnlyAny<0>>
               && std::derived_from<T, EntryBase>
               && (std::default_initializable<T>
                   || std::derived_from<T, internal::BuildableEntryBase>)
               && !std::derived_from<T, EntryBuilderBase>;

export template <typename T>
concept decays_to_entry_c = entry_c<std::decay_t<T>>;

}   // namespace ddge::app::v2
