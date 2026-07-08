export module ddge.scheduler.accessors.events.Event;

import ddge.events.event_c;

namespace ddge::scheduler::accessors {

inline namespace events {

export template <ddge::events::event_c>
struct Event {};

}   // namespace events

}   // namespace ddge::scheduler::accessors
