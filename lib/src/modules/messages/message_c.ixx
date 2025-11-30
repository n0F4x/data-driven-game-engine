export module ddge.modules.messages.message_c;

import ddge.utility.meta.concepts.naked;
import ddge.utility.meta.concepts.storable;

namespace ddge::messages {

export template <typename T>
concept message_c = util::meta::storable_c<T> && util::meta::naked_c<T>;

}   // namespace ddge::messages
