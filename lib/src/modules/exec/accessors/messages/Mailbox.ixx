module;

#include <functional>

export module ddge.modules.exec.accessors.messages.Mailbox;

import ddge.modules.exec.accessors.messages.MessageManager;
import ddge.modules.exec.locks.CriticalSectionType;
import ddge.modules.exec.locks.Lock;
import ddge.modules.exec.locks.LockGroup;
import ddge.modules.messages.MessageManager;

namespace ddge::exec::accessors {

inline namespace messages {

export class Mailbox {
public:
    constexpr static auto lock_group() -> LockGroup;

    explicit Mailbox(ddge::messages::MessageManager& message_manager);

    auto clear_messages() const -> void;

private:
    std::reference_wrapper<ddge::messages::MessageManager> m_message_manager_ref;
};

}   // namespace messages

}   // namespace ddge::exec::accessors

constexpr auto ddge::exec::accessors::messages::Mailbox::lock_group() -> LockGroup
{
    LockGroup lock_group;
    lock_group.expand<MessageManager>(Lock{ CriticalSectionType::eExclusive });
    return lock_group;
}

ddge::exec::accessors::messages::Mailbox::Mailbox(
    ddge::messages::MessageManager& message_manager
)
    : m_message_manager_ref{ message_manager }
{}

auto ddge::exec::accessors::messages::Mailbox::clear_messages() const -> void
{
    m_message_manager_ref.get().clear_messages();
}
