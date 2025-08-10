module;

#include <flat_map>
#include <typeindex>
#include <utility>

#include "utility/contracts_macros.hpp"

export module ddge.modules.messages.Plugin;

import ddge.modules.app;
import ddge.modules.messages.Addon;
import ddge.modules.messages.ErasedMessageBuffer;
import ddge.modules.messages.message_c;

import ddge.utility.contracts;

namespace ddge::messages {

export class Plugin {
public:
    template <ddge::messages::message_c Message_T, typename Self_T>
    auto register_message(this Self_T&&) -> Self_T;

    template <ddge::app::decays_to_app_c App_T>
    [[nodiscard]]
    auto build(App_T&& app) && -> ddge::app::add_on_t<App_T, Addon>;

    template <ddge::messages::message_c Message_T>
    [[nodiscard]]
    auto manages_message() const noexcept -> bool;

private:
    std::flat_map<std::type_index, ddge::messages::ErasedMessageBuffer> m_message_buffers;
};

}   // namespace ddge::messages

template <ddge::messages::message_c Message_T, typename Self_T>
auto ddge::messages::Plugin::register_message(this Self_T&& self) -> Self_T
{
    Plugin& this_self{ static_cast<Plugin&>(self) };
    PRECOND((!this_self.manages_message<Message_T>()));
    this_self.m_message_buffers
        .try_emplace(typeid(Message_T), std::in_place_type<Message_T>);
    return std::forward<Self_T>(self);
}

template <ddge::app::decays_to_app_c App_T>
auto ddge::messages::Plugin::build(App_T&& app) && -> ddge::app::add_on_t<App_T, Addon>
{
    return std::forward<App_T>(app).add_on(
        Addon{ .message_manager{ std::move(m_message_buffers) } }
    );
}

template <ddge::messages::message_c Message_T>
auto ddge::messages::Plugin::manages_message() const noexcept -> bool
{
    return m_message_buffers.contains(typeid(Message_T));
}
