#include "engine/Controller.hpp"

namespace engine {

Controller::Controller(SceneGraphType& t_sceneGraph,
                       StateMachineType& t_stateMachine) noexcept
    : m_sceneGraph{ t_sceneGraph },
      m_stateMachine{ t_stateMachine } {}

auto Controller::make_scene() const noexcept -> SceneGraphType::SceneType {
    return m_sceneGraph.make_scene();
}

auto Controller::running() const noexcept -> bool {
    return m_stateMachine.running();
};

void Controller::quit() noexcept {
    m_stateMachine.exit();
}

void Controller::transition() noexcept {
    m_stateMachine.transition();
}

void Controller::transition_to(
    StateMachineType::StateIdType t_stateId) noexcept {
    m_stateMachine.set_next_state(t_stateId);
}

void Controller::transition_to_prev() noexcept {
    m_stateMachine.set_next_state_as_previous();
}

}   // namespace engine
