export module ddge.modules.exec.accessors.messages:locks.MessageManagerLock;

import ddge.modules.exec.locks.Lock;

namespace ddge::exec::accessors {

inline namespace messages {

export struct MessageManagerLock : Lock<MessageManagerLock> {};

}   // namespace messages

}   // namespace ddge::exec::accessors
