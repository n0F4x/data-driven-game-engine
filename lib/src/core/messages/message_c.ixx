export module core.messages.message_c;

import core.store.item_c;

namespace core::messages {

export template <typename T>
concept message_c = store::item_c<T>;

}   // namespace core::messages
