export module core.events.event_c;

import utility.meta.concepts.naked;
import utility.meta.concepts.storable;

namespace core::events {

export template <typename T>
concept event_c = util::meta::storable_c<T> && util::meta::naked_c<T>;

}   // namespace core::events
