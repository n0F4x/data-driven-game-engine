module;

#include <utility>

export module plugins.messages;

import app.Builder;
import app.decays_to_app_c;
import app.decays_to_builder_c;
import core.messages;

import addons.Messages;

import utility.meta.type_traits.type_list.type_list_contains;
import utility.TypeList;

namespace plugins {

export struct MessagesTag {};

export template <core::messages::message_c... Messages_T>
class BasicMessages : public MessagesTag {
public:
    template <core::messages::message_c Message_T, app::decays_to_builder_c Self_T>
        requires(!util::meta::
                     type_list_contains_v<util::TypeList<Messages_T...>, Message_T>)
    constexpr auto register_message(this Self_T&&);

    template <app::decays_to_app_c App_T>
    [[nodiscard]]
    constexpr auto build(App_T&& app);
};

export using Messages = BasicMessages<>;

export inline constexpr Messages messages;

}   // namespace plugins

template <core::messages::message_c... Messages_T>
template <core::messages::message_c Message_T, app::decays_to_builder_c Self_T>
    requires(!util::meta::type_list_contains_v<util::TypeList<Messages_T...>, Message_T>)
constexpr auto plugins::BasicMessages<Messages_T...>::register_message(this Self_T&& self)
{
    return app::swap_plugin<BasicMessages>(std::forward<Self_T>(self), [](auto&&) {
        return BasicMessages<Messages_T..., Message_T>{};
    });
}

template <core::messages::message_c... Messages_T>
template <app::decays_to_app_c App_T>
constexpr auto plugins::BasicMessages<Messages_T...>::build(App_T&& app)
{
    return std::forward<App_T>(app).add_on(addons::Messages<Messages_T...>{});
}
