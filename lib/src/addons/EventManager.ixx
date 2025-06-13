export module addons.EventManager;

import core.events;

namespace addons {

export struct EventManagerTag {};

export template <core::events::event_c... Events_T>
struct EventManager : EventManagerTag {
    core::events::EventManager<Events_T...> event_manager;
};

}   // namespace addons
