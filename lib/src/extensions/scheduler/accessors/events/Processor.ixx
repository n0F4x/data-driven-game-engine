module;

#include <concepts>
#include <functional>
#include <utility>

export module extensions.scheduler.accessors.events.Processor;

namespace extensions::scheduler::accessors::events {

export class Processor {
public:
    template <typename AdvanceFrame_T>
        requires std::
            same_as<std::invoke_result_t<const std::remove_cvref_t<AdvanceFrame_T>&>, void>
        explicit Processor(AdvanceFrame_T&& advance_frame);

    auto process_events() const -> void;

private:
    std::function<void()> m_advance_frame;
};

}   // namespace extensions::scheduler::accessors::events

template <typename AdvanceFrame_T>
    requires std::
        same_as<std::invoke_result_t<const std::remove_cvref_t<AdvanceFrame_T>&>, void>
    extensions::scheduler::accessors::events::Processor::Processor(
        AdvanceFrame_T&& advance_frame
    )
    : m_advance_frame{ std::forward<AdvanceFrame_T>(advance_frame) }
{}

auto extensions::scheduler::accessors::events::Processor::process_events() const -> void
{
    m_advance_frame();
}
