#include "engine/Controller.hpp"

#include "engine/StateMachine.hpp"


void Controller::quit() noexcept {
    stateMachine.exit();
}

void Controller::transition_to(Id state) noexcept {
    stateMachine.transition_to(state);
}

void Controller::transition_to_prev() noexcept {
    stateMachine.transition_to_prev();
}

