module;

#include <functional>

export module ddge.scheduler.accessors.messages.Mailbox;

import ddge.scheduler.accessors.messages.MessageManager;
import ddge.scheduler.locks.CriticalSectionType;
import ddge.scheduler.locks.Lock;
import ddge.scheduler.locks.LockGroup;
import ddge.messages.MessageManager;

namespace ddge::scheduler::accessors {

inline namespace messages {

export class Mailbox {
public:
    constexpr static auto lock_group() -> const LockGroup&;

    explicit Mailbox(ddge::messages::MessageManager& message_manager);

    auto clear_messages() const -> void;

private:
    std::reference_wrapper<ddge::messages::MessageManager> m_message_manager_ref;
};

}   // namespace messages

}   // namespace ddge::scheduler::accessors

constexpr auto ddge::scheduler::accessors::messages::Mailbox::lock_group()
    -> const LockGroup&
{
    static const LockGroup lock_group{ [] -> LockGroup {
        LockGroup          result;
        result.expand<MessageManager>(Lock{ CriticalSectionType::eExclusive });
        return result;
    }() };

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
