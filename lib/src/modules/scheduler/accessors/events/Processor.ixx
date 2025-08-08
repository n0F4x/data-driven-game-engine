module;

#include <functional>

export module modules.scheduler.accessors.events.Processor;

import modules.events.EventManager;

namespace modules::scheduler::accessors {

inline namespace events {

export class Processor {
public:
    explicit Processor(modules::events::EventManager& event_manager);

    auto process_events() const -> void;

private:
    std::reference_wrapper<modules::events::EventManager> m_event_manager_ref;
};

}   // namespace events

}   // namespace modules::scheduler::accessors

modules::scheduler::accessors::events::Processor::Processor(
    modules::events::EventManager& event_manager
)
    : m_event_manager_ref{ event_manager }
{}

auto modules::scheduler::accessors::events::Processor::process_events() const -> void
{
    m_event_manager_ref.get().process_events();
}
