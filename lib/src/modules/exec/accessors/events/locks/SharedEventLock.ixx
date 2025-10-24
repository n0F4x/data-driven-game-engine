export module ddge.modules.exec.accessors.events:locks.SharedEventLock;

import :locks.EventManagerLock;

import ddge.modules.events.event_c;
import ddge.modules.exec.locks.CompositeLock;

namespace ddge::exec::accessors {

inline namespace events {

export template <ddge::events::event_c Event_T>
struct SharedEventLock
    : CompositeLock<SharedEventLock<Event_T>, LockDependencyList<EventManagerLock>> {};

}   // namespace events

}   // namespace ddge::exec::accessors
