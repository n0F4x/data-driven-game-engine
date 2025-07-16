module;

#include <concepts>
#include <functional>
#include <type_traits>

export module extensions.scheduler.argument_providers.MessageProvider;

import app;

import core.messages.MessageManager;

import extensions.scheduler.accessors.messages;

import utility.meta.concepts.decays_to;
import utility.meta.concepts.specialization_of;
import utility.meta.concepts.type_list.type_list_all_of;
import utility.meta.type_traits.type_list.type_list_transform;
import utility.meta.type_traits.underlying;

namespace extensions::scheduler::argument_providers {

export template <
    util::meta::specialization_of_c<core::messages::MessageManager> MessageManager_T,
    typename MessagesAddon_T>
class MessageProvider {
    template <typename Message_T>
    struct IsRegistered
        : std::bool_constant<MessageManager_T::template registered<Message_T>()> {};

    template <typename Accessor_T>
    constexpr static bool all_registered = util::meta::type_list_all_of_c<
        util::meta::type_list_transform_t<Accessor_T, std::remove_cvref>,
        IsRegistered>;

public:
    template <app::has_addons_c<MessagesAddon_T> App_T>
    constexpr explicit MessageProvider(App_T& app);

    template <util::meta::decays_to_c<accessors::messages::Mailbox> Accessor_T>
    [[nodiscard]]
    auto provide() const -> accessors::messages::Mailbox;

    template <typename Accessor_T>
        requires util::meta::specialization_of_c<
            std::remove_cvref_t<Accessor_T>,
            accessors::messages::Receiver>   //
    [[nodiscard]]
    constexpr auto provide() const -> std::remove_cvref_t<Accessor_T>
        requires(all_registered<std::remove_cvref_t<Accessor_T>>);

    template <typename Accessor_T>
        requires util::meta::specialization_of_c<
            std::remove_cvref_t<Accessor_T>,
            accessors::messages::Sender>   //
    [[nodiscard]]
    constexpr auto provide() const -> std::remove_cvref_t<Accessor_T>
        requires(all_registered<std::remove_cvref_t<Accessor_T>>);

private:
    std::reference_wrapper<MessageManager_T> m_message_manager_ref;
};

}   // namespace extensions::scheduler::argument_providers

template <
    util::meta::specialization_of_c<core::messages::MessageManager> MessageManager_T,
    typename MessagesAddon_T>
template <app::has_addons_c<MessagesAddon_T> App_T>
constexpr extensions::scheduler::argument_providers::
    MessageProvider<MessageManager_T, MessagesAddon_T>::MessageProvider(App_T& app)
    : m_message_manager_ref{ app.message_manager }
{}

template <
    util::meta::specialization_of_c<core::messages::MessageManager> MessageManager_T,
    typename MessagesAddon_T>
template <util::meta::decays_to_c<extensions::scheduler::accessors::messages::Mailbox>
              Accessor_T>
auto extensions::scheduler::argument_providers::
    MessageProvider<MessageManager_T, MessagesAddon_T>::provide() const
    -> extensions::scheduler::accessors::messages::Mailbox
{
    return std::remove_cvref_t<Accessor_T>{ m_message_manager_ref.get() };
}

template <
    util::meta::specialization_of_c<core::messages::MessageManager> MessageManager_T,
    typename MessagesAddon_T>
template <typename Accessor_T>
    requires util::meta::specialization_of_c<
        std::remove_cvref_t<Accessor_T>,
        extensions::scheduler::accessors::messages::Receiver>
constexpr auto extensions::scheduler::argument_providers::
    MessageProvider<MessageManager_T, MessagesAddon_T>::provide() const
    -> std::remove_cvref_t<Accessor_T>
    requires(all_registered<std::remove_cvref_t<Accessor_T>>)
{
    return std::remove_cvref_t<Accessor_T>{
        m_message_manager_ref.get()
            .template message_buffer<
                util::meta::underlying_t<std::remove_cvref_t<Accessor_T>>>()
    };
}

template <
    util::meta::specialization_of_c<core::messages::MessageManager> MessageManager_T,
    typename MessagesAddon_T>
template <typename Accessor_T>
    requires util::meta::specialization_of_c<
        std::remove_cvref_t<Accessor_T>,
        extensions::scheduler::accessors::messages::Sender>
constexpr auto extensions::scheduler::argument_providers::
    MessageProvider<MessageManager_T, MessagesAddon_T>::provide() const
    -> std::remove_cvref_t<Accessor_T>
    requires(all_registered<std::remove_cvref_t<Accessor_T>>)
{
    return std::remove_cvref_t<Accessor_T>{ m_message_manager_ref.get() };
}
