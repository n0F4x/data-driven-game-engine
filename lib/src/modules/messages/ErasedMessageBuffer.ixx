module;

#include <functional>
#include <type_traits>
#include <utility>

export module ddge.modules.messages.ErasedMessageBuffer;

import ddge.modules.messages.MessageBuffer;
import ddge.modules.messages.message_c;

import ddge.utility.containers.Any;

namespace ddge::messages {

export class ErasedMessageBuffer;

struct ErasedMessageBufferOperations {
    using SwapBuffers = auto (&)(ErasedMessageBuffer&) -> void;

    std::reference_wrapper<std::remove_reference_t<SwapBuffers>> clear;
};

template <message_c>
struct ErasedMessageBufferTraits {
    static auto clear(ErasedMessageBuffer&) -> void;

    constexpr static ErasedMessageBufferOperations operations{
        .clear{ clear },
    };
};

struct DummyMessage {};

export class ErasedMessageBuffer : public util::BasicAny<
                                       sizeof(MessageBuffer<DummyMessage>),
                                       alignof(MessageBuffer<DummyMessage>)> {
    using Base = util::
        BasicAny<sizeof(MessageBuffer<DummyMessage>), alignof(MessageBuffer<DummyMessage>)>;

public:
    template <message_c Message_T>
    explicit ErasedMessageBuffer(std::in_place_type_t<Message_T>);

    auto clear() -> void;

private:
    std::reference_wrapper<const ErasedMessageBufferOperations> m_operations;
};

}   // namespace ddge::messages

template <ddge::messages::message_c Message_T>
auto ddge::messages::ErasedMessageBufferTraits<Message_T>::clear(ErasedMessageBuffer& that)
    -> void
{
    util::any_cast<MessageBuffer<Message_T>>(that).clear();
}

template <ddge::messages::message_c Message_T>
ddge::messages::ErasedMessageBuffer::ErasedMessageBuffer(std::in_place_type_t<Message_T>)
    : Base{ std::in_place_type<MessageBuffer<Message_T>> },
      m_operations{ ErasedMessageBufferTraits<Message_T>::operations }
{}

module :private;

auto ddge::messages::ErasedMessageBuffer::clear() -> void
{
    m_operations.get().clear(*this);
}
