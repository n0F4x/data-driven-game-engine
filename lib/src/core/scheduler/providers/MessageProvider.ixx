module;

#include <concepts>
#include <format>
#include <functional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module core.scheduler.providers.MessageProvider;

import addons.Messages;

import app;

import core.messages;
import core.scheduler.ProviderFor;
import core.store.Store;

import core.scheduler.accessors.messages;
import core.scheduler.ProviderOf;

import utility.contracts;
import utility.meta.algorithms.apply;
import utility.meta.algorithms.for_each;
import utility.meta.concepts.specialization_of;
import utility.meta.reflection.name_of;

namespace core::scheduler::providers {

class MessageProvider {
public:
    template <app::has_addons_c<addons::Messages> App_T>
    constexpr explicit MessageProvider(App_T& app);

    template <std::same_as<accessors::messages::Mailbox>>
    [[nodiscard]]
    auto provide() const -> accessors::messages::Mailbox;

    template <util::meta::specialization_of_c<accessors::messages::Receiver> Receiver_T>
    [[nodiscard]]
    constexpr auto provide() const -> Receiver_T;

    template <util::meta::specialization_of_c<accessors::messages::Sender> Sender_T>
    [[nodiscard]]
    constexpr auto provide() const -> Sender_T;

private:
    std::reference_wrapper<core::messages::MessageManager> m_message_manager_ref;
};

}   // namespace core::scheduler::providers

template <>
struct core::scheduler::ProviderOf<addons::Messages>
    : std::type_identity<core::scheduler::providers::MessageProvider> {};

template <>
struct core::scheduler::ProviderFor<core::scheduler::accessors::messages::Mailbox>
    : std::type_identity<core::scheduler::providers::MessageProvider> {};

template <typename Event_T>
struct core::scheduler::
    ProviderFor<core::scheduler::accessors::messages::Receiver<Event_T>>
    : std::type_identity<core::scheduler::providers::MessageProvider> {};

template <typename... Events_T>
struct core::scheduler::
    ProviderFor<core::scheduler::accessors::messages::Sender<Events_T...>>
    : std::type_identity<core::scheduler::providers::MessageProvider> {};

template <app::has_addons_c<addons::Messages> App_T>
constexpr core::scheduler::providers::MessageProvider::MessageProvider(App_T& app)
    : m_message_manager_ref{ app.message_manager }
{}

template <std::same_as<core::scheduler::accessors::messages::Mailbox>>
auto core::scheduler::providers::MessageProvider::provide() const
    -> core::scheduler::accessors::messages::Mailbox
{
    return core::scheduler::accessors::messages::Mailbox{ m_message_manager_ref };
}

template <util::meta::specialization_of_c<
    core::scheduler::accessors::messages::Receiver> Receiver_T>
constexpr auto core::scheduler::providers::MessageProvider::provide() const
    -> Receiver_T
{
    using Message = typename Receiver_T::Message;

    PRECOND(
        (m_message_manager_ref.get().manages_message<Message>()),
        std::format("Message {} was not registered", util::meta::name_of<Message>())
    );

    return Receiver_T{ m_message_manager_ref.get().message_buffer<Message>() };
}

template <util::meta::specialization_of_c<
    core::scheduler::accessors::messages::Sender> Sender_T>
constexpr auto core::scheduler::providers::MessageProvider::provide() const
    -> Sender_T
{
    using Messages = typename Sender_T::Messages;

    return util::meta::apply<Messages>([this]<typename... Messages_T> -> Sender_T {
        PRECOND((m_message_manager_ref.get().manages_message<Messages_T>() && ...));

        return Sender_T{ m_message_manager_ref.get().message_buffer<Messages_T>()... };
    });
}
