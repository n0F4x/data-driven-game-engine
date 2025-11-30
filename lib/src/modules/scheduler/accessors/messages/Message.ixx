export module ddge.modules.scheduler.accessors.messages.Message;

import ddge.modules.messages.message_c;

namespace ddge::scheduler::accessors {

inline namespace messages {

export template <::ddge::messages::message_c>
struct Message {};

}   // namespace messages

}   // namespace ddge::scheduler::accessors
