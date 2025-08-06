module;

#include <flat_map>
#include <typeindex>
#include <utility>

#include "utility/contracts_macros.hpp"

export module plugins.messages;

import addons.Messages;

import app;

import core.messages;

import utility.contracts;

namespace plugins {

export class Messages {
public:
    template <core::messages::message_c Message_T, typename Self_T>
    auto register_message(this Self_T&&) -> Self_T;

    template <app::decays_to_app_c App_T>
    [[nodiscard]]
    auto build(App_T&& app) && -> app::add_on_t<App_T, addons::Messages>;

    template <core::messages::message_c Message_T>
    [[nodiscard]]
    auto manages_message() const noexcept -> bool;

private:
    std::flat_map<std::type_index, core::messages::ErasedMessageBuffer> m_message_buffers;
};

}   // namespace plugins

template <core::messages::message_c Message_T, typename Self_T>
auto plugins::Messages::register_message(this Self_T&& self) -> Self_T
{
    Messages& this_self{ static_cast<Messages&>(self) };
    PRECOND((!this_self.manages_message<Message_T>()));
    this_self.m_message_buffers
        .try_emplace(typeid(Message_T), std::in_place_type<Message_T>);
    return std::forward<Self_T>(self);
}

template <app::decays_to_app_c App_T>
auto plugins::Messages::build(App_T&& app) && -> app::add_on_t<App_T, addons::Messages>
{
    return std::forward<App_T>(app).add_on(
        addons::Messages{ .message_manager{ std::move(m_message_buffers) } }
    );
}

template <core::messages::message_c Message_T>
auto plugins::Messages::manages_message() const noexcept -> bool
{
    return m_message_buffers.contains(typeid(Message_T));
}
