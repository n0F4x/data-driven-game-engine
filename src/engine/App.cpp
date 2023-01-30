#include "engine/App.hpp"

#include <algorithm>
#include <iostream>

#include "engine/Controller.hpp"
#include "framework/State.hpp"

namespace engine {

App::App(std::string_view t_name,
         SceneGraph&& t_sceneGraph,
         Schedule&& t_schedule,
         StateMachine&& t_stateMachine) noexcept
    : m_name{ t_name },
      m_sceneGraph{ std::move(t_sceneGraph) },
      m_schedule{ std::move(t_schedule) },
      m_stateMachine{ std::move(t_stateMachine) } {}

void App::run() {
    std::cout << m_name << " is running...\n";

    m_stateMachine.start();

    Controller controller{ m_sceneGraph, m_stateMachine };
    m_schedule.execute(controller);

    m_stateMachine.exit();
}

App::Builder::operator App() noexcept {
    return build();
}

auto App::Builder::build() noexcept -> App {
    return App{ m_name,
                std::move(m_sceneGraph),
                std::move(m_schedule),
                std::move(m_stateMachine) };
}

auto App::Builder::set_name(std::string_view t_name) noexcept -> Builder& {
    m_name = t_name;
    return *this;
}

auto App::Builder::set_scene_graph(SceneGraph&& t_sceneGraph) -> Builder& {
    m_sceneGraph = std::move(t_sceneGraph);
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

}   // namespace engine
