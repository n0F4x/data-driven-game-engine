module;

#include <concepts>
#include <functional>
#include <utility>

export module extensions.scheduler.accessors.events.ClearEventsCommand;

namespace extensions::scheduler::accessors::events {

export class ClearEventsCommand {
public:
    template <typename ClearEvents_T>
        requires std::
            same_as<std::invoke_result_t<const std::remove_cvref_t<ClearEvents_T>&>, void>
        explicit ClearEventsCommand(ClearEvents_T&& clear_events);

    auto operator()() const -> void;

private:
    std::function<void()> m_clear_events;
};

}   // namespace extensions::scheduler::accessors::events

template <typename ClearEvents_T>
    requires std::
        same_as<std::invoke_result_t<const std::remove_cvref_t<ClearEvents_T>&>, void>
    extensions::scheduler::accessors::events::ClearEventsCommand::
        ClearEventsCommand(ClearEvents_T&& clear_events)
    : m_clear_events{ std::forward<ClearEvents_T>(clear_events) }
{}

auto extensions::scheduler::accessors::events::ClearEventsCommand::
    operator()() const -> void
{
    m_clear_events();
}
