module;

#include <type_traits>

export module ddge.modules.events.decays_to_event_c;

import ddge.modules.events.event_c;

namespace ddge::events {

export template <typename T>
concept decays_to_event_c = event_c<std::decay_t<T>>;

}   // namespace ddge::events
