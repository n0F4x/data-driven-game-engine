export module ddge.modules.exec.accessors.messages:locks.ExclusiveMessageLock;

import :locks.MessageManagerLock;
import :locks.SharedMessageLock;

import ddge.modules.messages.message_c;
import ddge.modules.exec.locks.CompositeLock;

namespace ddge::exec::accessors {

inline namespace messages {

export template <ddge::messages::message_c Message_T>
struct ExclusiveMessageLock
    : CompositeLock<
          ExclusiveMessageLock<Message_T>,
          LockDependencyList<SharedMessageLock<Message_T>, MessageManagerLock>> {};

}   // namespace messages

}   // namespace ddge::exec::accessors
