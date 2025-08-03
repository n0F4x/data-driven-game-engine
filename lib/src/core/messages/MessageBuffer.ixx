module;

#include <vector>

export module core.messages.MessageBuffer;

import core.messages.message_c;

namespace core::messages {

export template <message_c Message_T>
using MessageBuffer = std::vector<Message_T>;

}   // namespace core::messages
