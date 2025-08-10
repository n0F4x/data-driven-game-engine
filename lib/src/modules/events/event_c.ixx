export module ddge.modules.events.event_c;

import ddge.modules.store.item_c;

namespace ddge::events {

export template <typename T>
concept event_c = store::item_c<T>;

}   // namespace ddge::events
