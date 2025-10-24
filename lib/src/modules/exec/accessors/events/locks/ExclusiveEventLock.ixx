export module ddge.modules.exec.accessors.events:locks.ExclusiveEventLock;

import :locks.EventManagerLock;
import :locks.SharedEventLock;

import ddge.modules.events.event_c;
import ddge.modules.exec.locks.CompositeLock;

namespace ddge::exec::accessors {

inline namespace events {

export template <ddge::events::event_c Event_T>
struct ExclusiveEventLock
    : CompositeLock<
          ExclusiveEventLock<Event_T>,
          LockDependencyList<SharedEventLock<Event_T>, EventManagerLock>> {};

}   // namespace events

}   // namespace ddge::exec::accessors
