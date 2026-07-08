export module ddge.messages.message_c;

import ddge.util.meta.concepts.naked;
import ddge.util.meta.concepts.storable;

namespace ddge::messages {

export template <typename T>
concept message_c = util::meta::storable_c<T> && util::meta::naked_c<T>;

}   // namespace ddge::messages
