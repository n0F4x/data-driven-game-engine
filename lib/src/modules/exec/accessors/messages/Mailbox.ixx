module;

#include <functional>

export module ddge.modules.exec.accessors.messages.Mailbox;

import ddge.modules.messages.MessageManager;

namespace ddge::exec::accessors {

inline namespace messages {

export class Mailbox {
public:
    explicit Mailbox(ddge::messages::MessageManager& message_manager);

    auto clear_messages() const -> void;

private:
    std::reference_wrapper<ddge::messages::MessageManager> m_message_manager_ref;
};

}   // namespace messages

}   // namespace ddge::exec::accessors

ddge::exec::accessors::messages::Mailbox::Mailbox(
    ddge::messages::MessageManager& message_manager
)
    : m_message_manager_ref{ message_manager }
{}

auto ddge::exec::accessors::messages::Mailbox::clear_messages() const -> void
{
    m_message_manager_ref.get().clear_messages();
}
