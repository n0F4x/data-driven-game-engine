export module core.events.event_c;

import core.store.item_c;

namespace core::events {

export template <typename T>
concept event_c = store::item_c<T>;

}   // namespace core::events
