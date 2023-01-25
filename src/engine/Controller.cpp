#include "engine/Controller.hpp"

namespace engine {

void Controller::quit() noexcept {
    m_stateMachine.exit();
}

void Controller::transition_to(config::Id t_stateId) noexcept {
    m_stateMachine.transition_to(t_stateId);
}

void Controller::transition_to_prev() noexcept {
    m_stateMachine.transition_to_previous();
}

}   // namespace engine
