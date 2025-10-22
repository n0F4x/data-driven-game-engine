module;

#include <functional>

export module ddge.modules.exec.accessors.events.Processor;

import ddge.modules.events.EventManager;

namespace ddge::exec::accessors {

inline namespace events {

export class Processor {
public:
    explicit Processor(ddge::events::EventManager& event_manager);

    auto process_events() const -> void;

private:
    std::reference_wrapper<ddge::events::EventManager> m_event_manager_ref;
};

}   // namespace events

}   // namespace ddge::exec::accessors

ddge::exec::accessors::events::Processor::Processor(
    ddge::events::EventManager& event_manager
)
    : m_event_manager_ref{ event_manager }
{}

auto ddge::exec::accessors::events::Processor::process_events() const -> void
{
    m_event_manager_ref.get().process_events();
}
