#include "engine/Controller.hpp"

namespace engine {

Controller::Controller(SceneGraphType& t_sceneGraph,
                       StateMachineType& t_stateMachine) noexcept
    : m_sceneGraph{ t_sceneGraph },
      m_stateMachine{ t_stateMachine } {}

auto Controller::running() const noexcept -> bool {
    return m_running;
}

void Controller::quit() noexcept {
    m_running = false;
}

auto Controller::sceneGraph() noexcept -> SceneGraphType& {
    return m_sceneGraph;
}

auto Controller::stateMachine() noexcept -> StateMachineType& {
    return m_stateMachine;
}

}   // namespace engine
