module;

#include <concepts>
#include <format>
#include <functional>
#include <type_traits>

#include "utility/contracts_macros.hpp"

export module modules.scheduler.providers.MessageProvider;

import modules.app;

import modules.messages;
import modules.scheduler.ProviderFor;
import modules.store.Store;

import modules.scheduler.accessors.messages;
import modules.scheduler.ProviderOf;

import utility.contracts;
import utility.meta.algorithms.apply;
import utility.meta.algorithms.for_each;
import utility.meta.concepts.specialization_of;
import utility.meta.reflection.name_of;

namespace modules::scheduler::providers {

class MessageProvider {
public:
    template <modules::app::has_addons_c<messages::Addon> App_T>
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
    std::reference_wrapper<modules::messages::MessageManager> m_message_manager_ref;
};

}   // namespace modules::scheduler::providers

template <>
struct modules::scheduler::ProviderOf<modules::messages::Addon>
    : std::type_identity<modules::scheduler::providers::MessageProvider> {};

template <>
struct modules::scheduler::ProviderFor<modules::scheduler::accessors::messages::Mailbox>
    : std::type_identity<modules::scheduler::providers::MessageProvider> {};

template <typename Event_T>
struct modules::scheduler::
    ProviderFor<modules::scheduler::accessors::messages::Receiver<Event_T>>
    : std::type_identity<modules::scheduler::providers::MessageProvider> {};

template <typename... Events_T>
struct modules::scheduler::
    ProviderFor<modules::scheduler::accessors::messages::Sender<Events_T...>>
    : std::type_identity<modules::scheduler::providers::MessageProvider> {};

template <modules::app::has_addons_c<modules::messages::Addon> App_T>
constexpr modules::scheduler::providers::MessageProvider::MessageProvider(App_T& app)
    : m_message_manager_ref{ app.message_manager }
{}

template <std::same_as<modules::scheduler::accessors::messages::Mailbox>>
auto modules::scheduler::providers::MessageProvider::provide() const
    -> modules::scheduler::accessors::messages::Mailbox
{
    return modules::scheduler::accessors::messages::Mailbox{ m_message_manager_ref };
}

template <util::meta::specialization_of_c<
    modules::scheduler::accessors::messages::Receiver> Receiver_T>
constexpr auto modules::scheduler::providers::MessageProvider::provide() const
    -> Receiver_T
{
    using Message = typename Receiver_T::Message;

    PRECOND(
        (m_message_manager_ref.get().manages_message<Message>()),
        std::format("Message {} was not registered", util::meta::name_of<Message>())
    );

    return Receiver_T{ m_message_manager_ref.get().message_buffer<Message>() };
}

template <util::meta::specialization_of_c<modules::scheduler::accessors::messages::Sender>
              Sender_T>
constexpr auto modules::scheduler::providers::MessageProvider::provide() const -> Sender_T
{
    using Messages = typename Sender_T::Messages;

    return util::meta::apply<Messages>([this]<typename... Messages_T> -> Sender_T {
        PRECOND((m_message_manager_ref.get().manages_message<Messages_T>() && ...));

        return Sender_T{ m_message_manager_ref.get().message_buffer<Messages_T>()... };
    });
}
