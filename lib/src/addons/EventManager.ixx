export module addons.EventManager;

import core.events.event_c;
import core.events.EventManager;

namespace addons {

export struct EventManagerTag {};

export template <typename... Events_T>
struct EventManager : EventManagerTag {
    core::events::EventManager<Events_T...> event_manager;
};

}   // namespace addons
