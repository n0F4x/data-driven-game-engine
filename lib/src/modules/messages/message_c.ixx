export module ddge.modules.messages.message_c;

import ddge.modules.store.item_c;

namespace ddge::messages {

export template <typename T>
concept message_c = store::item_c<T>;

}   // namespace ddge::messages
