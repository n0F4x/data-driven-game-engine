module;

#include <functional>

export module modules.scheduler.accessors.messages.Mailbox;

import modules.messages.MessageManager;

namespace modules::scheduler::accessors {

inline namespace messages {

export class Mailbox {
public:
    explicit Mailbox(modules::messages::MessageManager& message_manager);

    auto clear_messages() const -> void;

private:
    std::reference_wrapper<modules::messages::MessageManager> m_message_manager_ref;
};

}   // namespace messages

}   // namespace modules::scheduler::accessors

modules::scheduler::accessors::messages::Mailbox::Mailbox(
    modules::messages::MessageManager& message_manager
)
    : m_message_manager_ref{ message_manager }
{}

auto modules::scheduler::accessors::messages::Mailbox::clear_messages() const -> void
{
    m_message_manager_ref.get().clear_messages();
}
