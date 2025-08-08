module;

#include <functional>

export module core.scheduler.accessors.events.Processor;

import core.events.EventManager;

namespace core::scheduler::accessors {

inline namespace events {

export class Processor {
public:
    explicit Processor(core::events::EventManager& event_manager);

    auto process_events() const -> void;

private:
    std::reference_wrapper<core::events::EventManager> m_event_manager_ref;
};

}   // namespace events

}   // namespace core::scheduler::accessors

core::scheduler::accessors::events::Processor::Processor(
    core::events::EventManager& event_manager
)
    : m_event_manager_ref{ event_manager }
{}

auto core::scheduler::accessors::events::Processor::process_events() const -> void
{
    m_event_manager_ref.get().process_events();
}
