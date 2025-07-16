export module core.messages.message_c;

import utility.meta.concepts.naked;
import utility.meta.concepts.storable;

namespace core::messages {

export template <typename T>
concept message_c = util::meta::storable_c<T> && util::meta::naked_c<T>;

}   // namespace core::messages
