export module ddge.modules.exec.accessors.messages:locks.SharedMessageLock;

import :locks.MessageManagerLock;

import ddge.modules.messages.message_c;
import ddge.modules.exec.locks.CompositeLock;

namespace ddge::exec::accessors {

inline namespace messages {

export template <ddge::messages::message_c Message_T>
struct SharedMessageLock
    : CompositeLock<SharedMessageLock<Message_T>, LockDependencyList<MessageManagerLock>> {};

}   // namespace messages

}   // namespace ddge::exec::accessors
