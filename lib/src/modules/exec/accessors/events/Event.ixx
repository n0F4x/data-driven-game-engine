export module ddge.modules.exec.accessors.events.Event;

import ddge.modules.events.event_c;

namespace ddge::exec::accessors {

inline namespace events {

export template <ddge::events::event_c>
struct Event {};

}   // namespace events

}   // namespace ddge::exec::accessors
