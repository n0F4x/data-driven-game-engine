module;

#include <functional>
#include <span>
#include <vector>

export module ddge.modules.scheduler.accessors.messages.Receiver;

import ddge.modules.scheduler.accessors.messages.Message;
import ddge.modules.scheduler.accessors.messages.MessageManager;
import ddge.modules.scheduler.locks.CriticalSectionType;
import ddge.modules.scheduler.locks.Lock;
import ddge.modules.scheduler.locks.LockGroup;
import ddge.modules.messages.message_c;

namespace ddge::scheduler::accessors {

inline namespace messages {

export template <ddge::messages::message_c Message_T>
class Receiver {
public:
    using Message = Message_T;

    constexpr static auto lock_group() -> const LockGroup&;

    constexpr explicit Receiver(const std::vector<Message_T>& message_buffer);

    [[nodiscard]]
    constexpr auto receive() const -> std::span<const Message_T>;

private:
    std::reference_wrapper<const std::vector<Message_T>> m_message_buffer_ref;
};

}   // namespace messages

}   // namespace ddge::scheduler::accessors

template <ddge::messages::message_c Message_T>
constexpr auto ddge::scheduler::accessors::messages::Receiver<Message_T>::lock_group()
    -> const LockGroup&
{
    static const LockGroup lock_group{ [] -> LockGroup {
        LockGroup          result;
        result.expand<::ddge::scheduler::accessors::messages::Message<Message>>(
            Lock{ CriticalSectionType::eShared }   //
        );
        result.expand<MessageManager>(Lock{ CriticalSectionType::eShared });
        return result;
    }() };

    return lock_group;
}

template <ddge::messages::message_c Message_T>
constexpr ddge::scheduler::accessors::messages::Receiver<Message_T>::Receiver(
    const std::vector<Message_T>& message_buffer
)
    : m_message_buffer_ref{ message_buffer }
{}

template <ddge::messages::message_c Message_T>
constexpr auto ddge::scheduler::accessors::messages::Receiver<Message_T>::receive() const
    -> std::span<const Message_T>
{
    return std::span{ m_message_buffer_ref.get() };
}
