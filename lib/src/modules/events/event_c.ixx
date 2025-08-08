export module modules.events.event_c;

import modules.store.item_c;

namespace modules::events {

export template <typename T>
concept event_c = store::item_c<T>;

}   // namespace modules::events
