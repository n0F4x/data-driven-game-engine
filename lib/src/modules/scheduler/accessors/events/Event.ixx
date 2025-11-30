export module ddge.modules.scheduler.accessors.events.Event;

import ddge.modules.events.event_c;

namespace ddge::scheduler::accessors {

inline namespace events {

export template <ddge::events::event_c>
struct Event {};

}   // namespace events

}   // namespace ddge::scheduler::accessors
