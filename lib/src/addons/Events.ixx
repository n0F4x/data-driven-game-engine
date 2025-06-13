export module addons.Events;

import core.events;

namespace addons {

export struct EventsTag {};

export template <core::events::event_c... Events_T>
struct Events : EventsTag {
    core::events::EventManager<Events_T...> event_manager;
};

}   // namespace addons
