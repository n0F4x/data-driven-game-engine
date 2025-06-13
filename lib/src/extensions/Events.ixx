module;

#include <utility>

export module extensions.Events;

import addons.Events;

import core.app.Builder;
import core.app.decays_to_app_c;
import core.app.decays_to_builder_c;
import core.events.event_c;

import utility.meta.type_traits.type_list.type_list_contains;
import utility.TypeList;

namespace extensions {

export struct EventsTag {};

export template <core::events::event_c... Events_T>
class BasicEvents : public EventsTag {
public:
    template <core::events::event_c Event_T, core::app::decays_to_builder_c Self_T>
        requires(!util::meta::type_list_contains_v<util::TypeList<Events_T...>, Event_T>)
    constexpr auto register_event(this Self_T&&);

    template <core::app::decays_to_app_c App_T>
    [[nodiscard]]
    constexpr auto build(App_T&& app);
};

export using Events = BasicEvents<>;

export inline constexpr Events events;

}   // namespace extensions

template <core::events::event_c... Events_T>
template <core::events::event_c Event_T, core::app::decays_to_builder_c Self_T>
    requires(!util::meta::type_list_contains_v<util::TypeList<Events_T...>, Event_T>)
constexpr auto extensions::BasicEvents<Events_T...>::register_event(
    this Self_T&& self
)
{
    return core::app::swap_extension<BasicEvents>(
        std::forward<Self_T>(self),
        [](auto&&) { return BasicEvents<Events_T..., Event_T>{}; }
    );
}

template <core::events::event_c... Events_T>
template <core::app::decays_to_app_c App_T>
constexpr auto extensions::BasicEvents<Events_T...>::build(App_T&& app)
{
    return std::forward<App_T>(app).add_on(addons::Events<Events_T...>{});
}
