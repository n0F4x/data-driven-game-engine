#include <algorithm>
#include <iostream>

#include "Controller.hpp"
#include "engine/state_machine/State.hpp"

namespace app {

template <class RendererType, class StateMachineType>
BasicApp<RendererType, StateMachineType>::BasicApp(std::string_view t_name,
         Renderer&& t_renderer,
         Schedule&& t_schedule,
         StateMachine&& t_stateMachine) noexcept
    : m_name{ t_name },
      m_renderer{ std::move(t_renderer) },
      m_schedule{ std::move(t_schedule) },
      m_stateMachine{ std::move(t_stateMachine) } {}

template <class RendererType, class StateMachineType>
void BasicApp<RendererType, StateMachineType>::run() {
    std::cout << m_name << " is running...\n";

    m_stateMachine.start();

    Controller controller{ m_stateMachine };
    m_schedule.execute(controller);

    m_stateMachine.exit();
}

template <class RendererType, class StateMachineType>
BasicApp<RendererType, StateMachineType>::Builder::operator BasicApp() noexcept {
    return build();
}

template <class RendererType, class StateMachineType>
auto BasicApp<RendererType, StateMachineType>::Builder::build() noexcept -> BasicApp {
    return BasicApp{ m_name,
                std::move(m_renderer),
                std::move(m_schedule),
                std::move(m_stateMachine) };
}

template <class RendererType, class StateMachineType>
auto BasicApp<RendererType, StateMachineType>::Builder::set_name(std::string_view t_name) noexcept -> Builder& {
    m_name = t_name;
    return *this;
}

template <class RendererType, class StateMachineType>
auto BasicApp<RendererType, StateMachineType>::Builder::set_renderer(Renderer&& t_renderer) -> Builder& {
    m_renderer = std::move(t_renderer);
    return *this;
}

template <class RendererType, class StateMachineType>
auto BasicApp<RendererType, StateMachineType>::Builder::set_schedule(Schedule&& t_schedule) -> Builder& {
    m_schedule = std::move(t_schedule);
    return *this;
}

template <class RendererType, class StateMachineType>
auto BasicApp<RendererType, StateMachineType>::Builder::set_state_machine(StateMachine&& t_stateMachine)
    -> Builder& {
    m_stateMachine = std::move(t_stateMachine);
    return *this;
}

}   // namespace app
