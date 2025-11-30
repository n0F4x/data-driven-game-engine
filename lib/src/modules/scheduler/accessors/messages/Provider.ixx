module;

#include <concepts>
#include <format>
#include <functional>
#include <type_traits>

#include "utility/contract_macros.hpp"

export module ddge.modules.scheduler.accessors.messages.Provider;

import ddge.modules.app;
import ddge.modules.messages;
import ddge.modules.scheduler.accessors.messages.Mailbox;
import ddge.modules.scheduler.accessors.messages.Receiver;
import ddge.modules.scheduler.accessors.messages.Sender;
import ddge.modules.scheduler.ProviderFor;
import ddge.modules.scheduler.ProviderOf;
import ddge.modules.store.Store;

import ddge.utility.contracts;
import ddge.utility.meta.algorithms.apply;
import ddge.utility.meta.algorithms.for_each;
import ddge.utility.meta.concepts.specialization_of;
import ddge.utility.meta.reflection.name_of;

namespace ddge::scheduler::accessors {

inline namespace messages {

export class Provider {
public:
    template <ddge::app::has_addons_c<ddge::messages::Addon> App_T>
    constexpr explicit Provider(App_T& app);

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
    std::reference_wrapper<ddge::messages::MessageManager> m_message_manager_ref;
};

}   // namespace messages

}   // namespace ddge::scheduler::accessors

template <>
struct ddge::scheduler::ProviderFor<ddge::messages::Addon>
    : std::type_identity<ddge::scheduler::accessors::messages::Provider> {};

template <>
struct ddge::scheduler::ProviderOf<ddge::scheduler::accessors::messages::Mailbox>
    : std::type_identity<ddge::scheduler::accessors::messages::Provider> {};

template <typename Event_T>
struct ddge::scheduler::ProviderOf<ddge::scheduler::accessors::messages::Receiver<Event_T>>
    : std::type_identity<ddge::scheduler::accessors::messages::Provider> {};

template <typename... Events_T>
struct ddge::scheduler::
    ProviderOf<ddge::scheduler::accessors::messages::Sender<Events_T...>>
    : std::type_identity<ddge::scheduler::accessors::messages::Provider> {};

template <ddge::app::has_addons_c<ddge::messages::Addon> App_T>
constexpr ddge::scheduler::accessors::messages::Provider::Provider(App_T& app)
    : m_message_manager_ref{ app.message_manager }
{}

template <std::same_as<ddge::scheduler::accessors::messages::Mailbox>>
auto ddge::scheduler::accessors::messages::Provider::provide() const
    -> ddge::scheduler::accessors::messages::Mailbox
{
    return ddge::scheduler::accessors::messages::Mailbox{ m_message_manager_ref };
}

template <ddge::util::meta::specialization_of_c<
    ddge::scheduler::accessors::messages::Receiver> Receiver_T>
constexpr auto ddge::scheduler::accessors::messages::Provider::provide() const
    -> Receiver_T
{
    using Message = typename Receiver_T::Message;

    PRECOND(
        (m_message_manager_ref.get().manages_message<Message>()),
        std::format("Message {} was not registered", util::meta::name_of<Message>())
    );

    return Receiver_T{ m_message_manager_ref.get().message_buffer<Message>() };
}

template <ddge::util::meta::
              specialization_of_c<ddge::scheduler::accessors::messages::Sender> Sender_T>
constexpr auto ddge::scheduler::accessors::messages::Provider::provide() const -> Sender_T
{
    using Messages = typename Sender_T::Messages;

    return util::meta::apply<Messages>([this]<typename... Messages_T> -> Sender_T {
        PRECOND((m_message_manager_ref.get().manages_message<Messages_T>() && ...));

        return Sender_T{ m_message_manager_ref.get().message_buffer<Messages_T>()... };
    });
}
