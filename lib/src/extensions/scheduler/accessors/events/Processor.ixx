module;

#include <functional>
#include <utility>

export module extensions.scheduler.accessors.events.Processor;

import core.events.EventManager;

import utility.containers.Any;
import utility.meta.concepts.specialization_of;

namespace extensions::scheduler::accessors {

inline namespace events {

export class Processor;

struct ProcessorOperations {
    using ProcessEventsFn = auto (&)(const Processor&) -> void;

    ProcessEventsFn process_events;
};

template <util::meta::specialization_of_c<core::events::EventManager> EventManager_T>
class ProcessorTraits {
public:
    static auto process_events(const Processor& processor) -> void;

    constexpr static ProcessorOperations operations{
        .process_events = process_events,
    };

private:
    [[nodiscard]]
    static auto unwrap(const Processor& processor) -> EventManager_T&;
};

export class Processor {
public:
    template <util::meta::specialization_of_c<core::events::EventManager> EventManager_T>
    explicit Processor(EventManager_T& event_manager);

    auto process_events() const -> void;

private:
    template <util::meta::specialization_of_c<core::events::EventManager> EventManager_T>
    friend class ProcessorTraits;

    util::BasicAny<sizeof(void*)> m_event_manager_ref;
    const ProcessorOperations&    m_operations;
};

}   // namespace events

}   // namespace extensions::scheduler::accessors

template <util::meta::specialization_of_c<core::events::EventManager> EventManager_T>
auto extensions::scheduler::accessors::events::ProcessorTraits<EventManager_T>::
    process_events(const Processor& processor) -> void
{
    unwrap(processor).process_events();
}

template <util::meta::specialization_of_c<core::events::EventManager> EventManager_T>
auto extensions::scheduler::accessors::events::ProcessorTraits<EventManager_T>::unwrap(
    const Processor& processor
) -> EventManager_T&
{
    return util::any_cast<std::reference_wrapper<EventManager_T>>(
               processor.m_event_manager_ref
    )
        .get();
}

template <util::meta::specialization_of_c<core::events::EventManager> EventManager_T>
extensions::scheduler::accessors::events::Processor::Processor(
    EventManager_T& event_manager
)
    : m_event_manager_ref{ std::in_place_type<std::reference_wrapper<EventManager_T>>,
                           event_manager },
      m_operations{ ProcessorTraits<EventManager_T>::operations }
{}

auto extensions::scheduler::accessors::events::Processor::process_events() const -> void
{
    m_operations.process_events(*this);
}
