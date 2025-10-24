export module ddge.modules.exec.accessors.events:locks.EventManagerLock;

import ddge.modules.exec.locks.Lock;

namespace ddge::exec::accessors {

inline namespace events {

export struct EventManagerLock : Lock<EventManagerLock> {};

}   // namespace events

}   // namespace ddge::exec::accessors
