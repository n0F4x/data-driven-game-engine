#include "engine/Controller.hpp"

#include "framework/StateMachine.hpp"

namespace engine {

void Controller::quit() noexcept {
    stateMachine.exit();
}

void Controller::transition_to(config::Id state) noexcept {
    stateMachine.transition_to(state);
}

void Controller::transition_to_prev() noexcept {
    stateMachine.transition_to_prev();
}

}   // namespace engine
