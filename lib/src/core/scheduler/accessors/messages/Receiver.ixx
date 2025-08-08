module;

#include <functional>
#include <span>
#include <vector>

export module core.scheduler.accessors.messages.Receiver;

import core.messages.message_c;

namespace core::scheduler::accessors {

inline namespace messages {

export template <core::messages::message_c Message_T>
class Receiver {
public:
    using Message = Message_T;

    constexpr explicit Receiver(const std::vector<Message_T>& message_buffer);

    [[nodiscard]]
    constexpr auto receive() const -> std::span<const Message_T>;

private:
    std::reference_wrapper<const std::vector<Message_T>> m_message_buffer_ref;
};

}   // namespace messages

}   // namespace core::scheduler::accessors

template <core::messages::message_c Message_T>
constexpr core::scheduler::accessors::messages::Receiver<Message_T>::Receiver(
    const std::vector<Message_T>& message_buffer
)
    : m_message_buffer_ref{ message_buffer }
{}

template <core::messages::message_c Message_T>
constexpr auto core::scheduler::accessors::messages::Receiver<Message_T>::receive(
) const -> std::span<const Message_T>
{
    return std::span{ m_message_buffer_ref.get() };
}
