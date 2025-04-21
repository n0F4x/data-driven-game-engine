export module core.events.event_c;

import utility.meta.concepts.decayed;

namespace core::events {

export template <typename T>
concept event_c = util::meta::decayed_c<T>;

}   // namespace core::events
