export module ddge.scheduler.accessors.messages.Message;

import ddge.messages.message_c;

namespace ddge::scheduler::accessors {

inline namespace messages {

export template <::ddge::messages::message_c>
struct Message {};

}   // namespace messages

}   // namespace ddge::scheduler::accessors
