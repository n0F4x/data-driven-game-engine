export module modules.messages.message_c;

import modules.store.item_c;

namespace modules::messages {

export template <typename T>
concept message_c = store::item_c<T>;

}   // namespace modules::messages
