module;

#include <vector>

export module ddge.modules.messages.MessageBuffer;

import ddge.modules.messages.message_c;

namespace ddge::messages {

export template <message_c Message_T>
using MessageBuffer = std::vector<Message_T>;

}   // namespace ddge::messages
