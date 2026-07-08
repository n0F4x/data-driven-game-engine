export module ddge.modules.events.event_c;

import ddge.util.meta.concepts.naked;
import ddge.util.meta.concepts.storable;

namespace ddge::events {

export template <typename T>
concept event_c = util::meta::storable_c<T> && util::meta::naked_c<T>;

}   // namespace ddge::events
