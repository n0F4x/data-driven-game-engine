export module ddge.modules.events.event_c;

import ddge.utility.meta.concepts.naked;
import ddge.utility.meta.concepts.storable;

namespace ddge::events {

export template <typename T>
concept event_c = util::meta::storable_c<T> && util::meta::naked_c<T>;

}   // namespace ddge::events
