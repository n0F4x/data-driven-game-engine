module;

#include <functional>
#include <type_traits>
#include <utility>

export module core.events.ErasedBufferedEventQueue;

import core.events.BufferedEventQueue;
import core.events.event_c;

import utility.containers.Any;

namespace core::events {

export class ErasedBufferedEventQueue;

struct ErasedBufferedEventQueueOperations {
    using SwapBuffers = auto (&)(ErasedBufferedEventQueue&) -> void;

    std::reference_wrapper<std::remove_reference_t<SwapBuffers>> swap_buffers;
};

template <event_c>
struct ErasedBufferedEventQueueTraits {
    static auto swap_buffers(ErasedBufferedEventQueue&) -> void;

    constexpr static ErasedBufferedEventQueueOperations operations{
        .swap_buffers{ swap_buffers },
    };
};

struct DummyEvent {};

export class ErasedBufferedEventQueue : public util::BasicAny<
                                            sizeof(BufferedEventQueue<DummyEvent>),
                                            alignof(BufferedEventQueue<DummyEvent>)> {
    using Base = util::BasicAny<
        sizeof(BufferedEventQueue<DummyEvent>),
        alignof(BufferedEventQueue<DummyEvent>)>;

public:
    template <event_c Event_T>
    explicit ErasedBufferedEventQueue(std::in_place_type_t<Event_T>);

    auto swap_buffers() -> void;

private:
    std::reference_wrapper<const ErasedBufferedEventQueueOperations> m_operations;
};

}   // namespace core::events

template <core::events::event_c Event_T>
auto core::events::ErasedBufferedEventQueueTraits<Event_T>::swap_buffers(
    ErasedBufferedEventQueue& that
) -> void
{
    util::any_cast<BufferedEventQueue<Event_T>>(that).swap_buffers();
}

template <core::events::event_c Event_T>
core::events::ErasedBufferedEventQueue::
    ErasedBufferedEventQueue(std::in_place_type_t<Event_T>)
    : Base{ std::in_place_type<BufferedEventQueue<Event_T>> },
      m_operations{ ErasedBufferedEventQueueTraits<Event_T>::operations }
{}

module :private;

auto core::events::ErasedBufferedEventQueue::swap_buffers() -> void
{
    m_operations.get().swap_buffers(*this);
}
