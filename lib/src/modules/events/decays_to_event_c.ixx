module;

#include <type_traits>

export module modules.events.decays_to_event_c;

import modules.events.event_c;

namespace modules::events {

export template <typename T>
concept decays_to_event_c = event_c<std::decay_t<T>>;

}   // namespace modules::events
