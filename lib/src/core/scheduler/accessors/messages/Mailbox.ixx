module;

#include <functional>

export module core.scheduler.accessors.messages.Mailbox;

import core.messages.MessageManager;

namespace core::scheduler::accessors {

inline namespace messages {

export class Mailbox {
public:
    explicit Mailbox(core::messages::MessageManager& message_manager);

    auto clear_messages() const -> void;

private:
    std::reference_wrapper<core::messages::MessageManager> m_message_manager_ref;
};

}   // namespace messages

}   // namespace core::scheduler::accessors

core::scheduler::accessors::messages::Mailbox::Mailbox(
    core::messages::MessageManager& message_manager
)
    : m_message_manager_ref{ message_manager }
{}

auto core::scheduler::accessors::messages::Mailbox::clear_messages() const -> void
{
    m_message_manager_ref.get().clear_messages();
}
