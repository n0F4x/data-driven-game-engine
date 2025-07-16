module;

#include <functional>
#include <span>
#include <vector>

export module extensions.scheduler.accessors.messages.Receiver;

import core.messages.message_c;

namespace extensions::scheduler::accessors::messages {

export template <core::messages::message_c Message_T>
class Receiver {
public:
    constexpr explicit Receiver(const std::vector<Message_T>& message_buffer);

    [[nodiscard]]
    constexpr auto receive() const -> std::span<const Message_T>;

private:
    std::reference_wrapper<const std::vector<Message_T>> m_message_buffer_ref;
};

}   // namespace extensions::scheduler::accessors::messages

template <core::messages::message_c Message_T>
constexpr extensions::scheduler::accessors::messages::Receiver<Message_T>::Receiver(
    const std::vector<Message_T>& message_buffer
)
    : m_message_buffer_ref{ message_buffer }
{}

template <core::messages::message_c Message_T>
constexpr auto extensions::scheduler::accessors::messages::Receiver<Message_T>::receive(
) const -> std::span<const Message_T>
{
    return std::span{ m_message_buffer_ref.get() };
}
