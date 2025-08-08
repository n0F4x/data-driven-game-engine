module;

#include <vector>

export module modules.messages.MessageBuffer;

import modules.messages.message_c;

namespace modules::messages {

export template <message_c Message_T>
using MessageBuffer = std::vector<Message_T>;

}   // namespace modules::messages
