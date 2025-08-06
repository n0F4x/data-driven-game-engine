module;

#include <functional>
#include <type_traits>
#include <utility>

export module core.messages.ErasedMessageBuffer;

import core.messages.MessageBuffer;
import core.messages.message_c;

import utility.containers.Any;

namespace core::messages {

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

}   // namespace core::messages

template <core::messages::message_c Message_T>
auto core::messages::ErasedMessageBufferTraits<Message_T>::clear(ErasedMessageBuffer& that)
    -> void
{
    util::any_cast<MessageBuffer<Message_T>>(that).clear();
}

template <core::messages::message_c Message_T>
core::messages::ErasedMessageBuffer::ErasedMessageBuffer(std::in_place_type_t<Message_T>)
    : Base{ std::in_place_type<MessageBuffer<Message_T>> },
      m_operations{ ErasedMessageBufferTraits<Message_T>::operations }
{}

module :private;

auto core::messages::ErasedMessageBuffer::clear() -> void
{
    m_operations.get().clear(*this);
}
