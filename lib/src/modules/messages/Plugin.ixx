module;

#include <flat_map>
#include <typeindex>
#include <utility>

#include "utility/contracts_macros.hpp"

export module modules.messages.Plugin;

import modules.app;
import modules.messages.Addon;
import modules.messages.ErasedMessageBuffer;
import modules.messages.message_c;

import utility.contracts;

namespace modules::messages {

export class Plugin {
public:
    template <modules::messages::message_c Message_T, typename Self_T>
    auto register_message(this Self_T&&) -> Self_T;

    template <modules::app::decays_to_app_c App_T>
    [[nodiscard]]
    auto build(App_T&& app) && -> modules::app::add_on_t<App_T, Addon>;

    template <modules::messages::message_c Message_T>
    [[nodiscard]]
    auto manages_message() const noexcept -> bool;

private:
    std::flat_map<std::type_index, modules::messages::ErasedMessageBuffer>
        m_message_buffers;
};

}   // namespace modules::messages

template <modules::messages::message_c Message_T, typename Self_T>
auto modules::messages::Plugin::register_message(this Self_T&& self) -> Self_T
{
    Plugin& this_self{ static_cast<Plugin&>(self) };
    PRECOND((!this_self.manages_message<Message_T>()));
    this_self.m_message_buffers
        .try_emplace(typeid(Message_T), std::in_place_type<Message_T>);
    return std::forward<Self_T>(self);
}

template <modules::app::decays_to_app_c App_T>
auto modules::messages::Plugin::build(
    App_T&& app
) && -> modules::app::add_on_t<App_T, Addon>
{
    return std::forward<App_T>(app).add_on(
        Addon{ .message_manager{ std::move(m_message_buffers) } }
    );
}

template <modules::messages::message_c Message_T>
auto modules::messages::Plugin::manages_message() const noexcept -> bool
{
    return m_message_buffers.contains(typeid(Message_T));
}
