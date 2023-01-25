#include "framework/StateMachine.hpp"

#include <ranges>

#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

void StateMachine::start() {
    if (std::ranges::empty(states)) {
        add_state(State::create<"START"_hs>());
    }
    currentState->entered();
}

void StateMachine::exit() noexcept {
    nextState = State::invalid_state();
}

void StateMachine::transition() noexcept {
    if (nextState != currentState) {
        currentState->exited();

        prevState = currentState;
        currentState = nextState;

        currentState->entered();
    }
}

void StateMachine::transition_to(Id state) noexcept {
    std::lock_guard guard{ *transitionLock };
    if (nextState == currentState) {
        if (auto iter{ states.find(state) }; iter != states.end()) {
            nextState = &iter->second;
        }
    }
}

void StateMachine::transition_to_prev() noexcept {
    std::lock_guard guard{ *transitionLock };
    if (nextState == currentState) {
        nextState = prevState;
    }
}

void StateMachine::add_state(State&& state) {
    if (std::empty(states)) {
        currentState =
            &states.try_emplace(state.get_id(), std::move(state)).first->second;
    } else {
        states.try_emplace(state.get_id(), std::move(state));
    }
}

auto StateMachine::running(const StateMachine& machine) noexcept -> bool {
    return !State::invalid(*machine.currentState);
}
