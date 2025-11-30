module;

#include <functional>

export module ddge.modules.scheduler.accessors.events.Processor;

import ddge.modules.events.EventManager;
import ddge.modules.scheduler.accessors.events.EventManager;
import ddge.modules.scheduler.locks.CriticalSectionType;
import ddge.modules.scheduler.locks.Lock;
import ddge.modules.scheduler.locks.LockGroup;

namespace ddge::scheduler::accessors {

inline namespace events {

export class Processor {
public:
    constexpr static auto lock_group() -> LockGroup;

    explicit Processor(ddge::events::EventManager& event_manager);

    auto process_events() const -> void;

private:
    std::reference_wrapper<ddge::events::EventManager> m_event_manager_ref;
};

}   // namespace events

}   // namespace ddge::scheduler::accessors

constexpr auto ddge::scheduler::accessors::events::Processor::lock_group() -> LockGroup
{
    LockGroup lock_group;
    lock_group.expand<EventManager>(Lock{ CriticalSectionType::eExclusive });
    return lock_group;
}

ddge::scheduler::accessors::events::Processor::Processor(
    ddge::events::EventManager& event_manager
)
    : m_event_manager_ref{ event_manager }
{}

auto ddge::scheduler::accessors::events::Processor::process_events() const -> void
{
    m_event_manager_ref.get().process_events();
}
