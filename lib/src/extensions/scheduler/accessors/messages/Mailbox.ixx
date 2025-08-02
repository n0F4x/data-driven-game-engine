module;

#include <functional>
#include <utility>

export module extensions.scheduler.accessors.messages.Mailbox;

import core.messages.MessageManager;

import utility.containers.Any;
import utility.meta.concepts.specialization_of;

namespace extensions::scheduler::accessors {

inline namespace messages {

export class Mailbox;

struct MailboxOperations {
    using ClearMessagesFn = auto (&)(const Mailbox&) -> void;

    ClearMessagesFn clear_messages;
};

template <util::meta::specialization_of_c<core::messages::MessageManager> MessageManager_T>
class MailboxTraits {
public:
    static auto clear_messages(const Mailbox& mailbox) -> void;

    constexpr static MailboxOperations operations{
        .clear_messages = clear_messages,
    };

private:
    [[nodiscard]]
    static auto unwrap(const Mailbox& mailbox) -> MessageManager_T&;
};

export class Mailbox {
public:
    template <
        util::meta::specialization_of_c<core::messages::MessageManager> MessageManager_T>
    explicit Mailbox(MessageManager_T& message_manager);

    auto clear_messages() const -> void;

private:
    template <
        util::meta::specialization_of_c<core::messages::MessageManager> MessageManager_T>
    friend class MailboxTraits;

    util::BasicAny<sizeof(void*)> m_message_manager_ref;
    const MailboxOperations&      m_operations;
};

}   // namespace messages

}   // namespace extensions::scheduler::accessors

template <util::meta::specialization_of_c<core::messages::MessageManager> MessageManager_T>
auto extensions::scheduler::accessors::messages::MailboxTraits<MessageManager_T>::
    clear_messages(const Mailbox& mailbox) -> void
{
    unwrap(mailbox).clear_messages();
}

template <util::meta::specialization_of_c<core::messages::MessageManager> MessageManager_T>
auto extensions::scheduler::accessors::messages::MailboxTraits<MessageManager_T>::unwrap(
    const Mailbox& mailbox
) -> MessageManager_T&
{
    return util::any_cast<std::reference_wrapper<MessageManager_T>>(
               mailbox.m_message_manager_ref
    )
        .get();
}

template <util::meta::specialization_of_c<core::messages::MessageManager> MessageManager_T>
extensions::scheduler::accessors::messages::Mailbox::Mailbox(
    MessageManager_T& message_manager
)
    : m_message_manager_ref{ std::in_place_type<std::reference_wrapper<MessageManager_T>>,
                             message_manager },
      m_operations{ MailboxTraits<MessageManager_T>::operations }
{}

auto extensions::scheduler::accessors::messages::Mailbox::clear_messages() const -> void
{
    m_operations.clear_messages(*this);
}
