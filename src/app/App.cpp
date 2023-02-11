#include "App.hpp"

#include <algorithm>
#include <iostream>

#include "Controller.hpp"
#include "engine/state_machine/State.hpp"

namespace app {

App::App(std::string_view t_name,
         Renderer&& t_renderer,
         Schedule&& t_schedule,
         StateMachine&& t_stateMachine) noexcept
    : m_name{ t_name },
      m_renderer{ std::move(t_renderer) },
      m_schedule{ std::move(t_schedule) },
      m_stateMachine{ std::move(t_stateMachine) } {}

void App::run() {
    std::cout << m_name << " is running...\n";

    m_stateMachine.start();

    Controller controller{ m_stateMachine };
    m_schedule.execute(controller);

    m_stateMachine.exit();
}

App::Builder::operator App() noexcept {
    return build();
}

auto App::Builder::build() noexcept -> App {
    return App{ m_name,
                std::move(m_renderer),
                std::move(m_schedule),
                std::move(m_stateMachine) };
}

auto App::Builder::set_name(std::string_view t_name) noexcept -> Builder& {
    m_name = t_name;
    return *this;
}

auto App::Builder::set_renderer(Renderer&& t_renderer) -> Builder& {
    m_renderer = std::move(t_renderer);
    return *this;
}

auto App::Builder::set_schedule(Schedule&& t_schedule) -> Builder& {
    m_schedule = std::move(t_schedule);
    return *this;
}

auto App::Builder::set_state_machine(StateMachine&& t_stateMachine)
    -> Builder& {
    m_stateMachine = std::move(t_stateMachine);
    return *this;
}

}   // namespace app
