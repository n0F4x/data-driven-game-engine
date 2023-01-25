#include "framework/StateMachine.hpp"

#include <ranges>

#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

namespace fw {

void StateMachine::start() {
    if (std::ranges::empty(m_states)) {
        add_state(State::create<"START"_hs>());
    }
    m_currentState->entered();
}

void StateMachine::exit() noexcept {
    m_nextState = State::invalid_state();
}

void StateMachine::transition() noexcept {
    if (m_nextState != m_currentState) {
        m_currentState->exited();

        m_prevState = m_currentState;
        m_currentState = m_nextState;

        m_currentState->entered();
    }
}

void StateMachine::transition_to(config::Id t_state) noexcept {
    std::lock_guard guard{ *m_transitionLock };
    if (m_nextState == m_currentState) {
        if (auto iter{ m_states.find(t_state) }; iter != m_states.end()) {
            m_nextState = &iter->second;
        }
    }
}

void StateMachine::transition_to_previous() noexcept {
    std::lock_guard guard{ *m_transitionLock };
    if (m_nextState == m_currentState) {
        m_nextState = m_prevState;
    }
}

void StateMachine::add_state(State&& t_state) {
    if (std::empty(m_states)) {
        m_currentState = &m_states.try_emplace(t_state.id(), std::move(t_state))
                              .first->second;
    } else {
        m_states.try_emplace(t_state.id(), std::move(t_state));
    }
}

auto StateMachine::running(const StateMachine& t_stateMachine) noexcept
    -> bool {
    return !State::invalid(*t_stateMachine.m_currentState);
}

}   // namespace fw
