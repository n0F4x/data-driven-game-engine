module;

#include <functional>

export module ddge.modules.scheduler.accessors.messages.Mailbox;

import ddge.modules.scheduler.accessors.messages.MessageManager;
import ddge.modules.scheduler.locks.CriticalSectionType;
import ddge.modules.scheduler.locks.Lock;
import ddge.modules.scheduler.locks.LockGroup;
import ddge.modules.messages.MessageManager;

namespace ddge::scheduler::accessors {

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

}   // namespace ddge::scheduler::accessors

constexpr auto ddge::scheduler::accessors::messages::Mailbox::lock_group() -> LockGroup
{
    LockGroup lock_group;
    lock_group.expand<MessageManager>(Lock{ CriticalSectionType::eExclusive });
    return lock_group;
}

ddge::scheduler::accessors::messages::Mailbox::Mailbox(
    ddge::messages::MessageManager& message_manager
)
    : m_message_manager_ref{ message_manager }
{}

auto ddge::scheduler::accessors::messages::Mailbox::clear_messages() const -> void
{
    m_message_manager_ref.get().clear_messages();
}
