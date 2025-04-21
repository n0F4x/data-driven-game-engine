module;

#include <type_traits>

export module core.events.decays_to_event_c;

import core.events.event_c;

namespace core::events {

export template <typename T>
concept decays_to_event_c = event_c<std::decay_t<T>>;

}   // namespace core::events
