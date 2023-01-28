#pragma once

#include <memory>
#include <unordered_map>

#include "framework/State.hpp"

namespace fw {

template <typename State>
class StateMachineBase final {
public:
    ///----------------///
    ///  Member types  ///
    ///----------------///
    using StateType = State;

    ///--------------------///
    ///  Member functions  ///
    ///--------------------///
    [[nodiscard]] auto running() const noexcept -> bool;
    void add_state(StateType&& t_state, bool t_setAsInitialState = false);
    void set_initial_state(StateType::IdType t_stateId) noexcept;
    void start() noexcept;
    void exit() noexcept;
    void transition() noexcept;
    void transition_to(StateType::IdType t_stateId) noexcept;
    void transition_to_previous() noexcept;

private:
    ///--------------------///
    ///  Member variables  ///
    ///--------------------///
    std::unordered_map<typename StateType::IdType, StateType> m_states;
    StateType m_invalidState = StateType::create();
    gsl::not_null<State*> m_nextState = &m_invalidState;
    gsl::not_null<State*> m_currentState = &m_invalidState;
    gsl::not_null<State*> m_previousState = &m_invalidState;
};

template <typename State>
auto StateMachineBase<State>::running() const noexcept -> bool {
    return m_currentState != &m_invalidState;
}

template <typename State>
void StateMachineBase<State>::add_state(StateType&& t_state,
                                        bool t_setAsInitialState) {
    if (auto [iter, success] =
            m_states.try_emplace(t_state.id(), std::move(t_state));
        success && t_setAsInitialState)
    {
        set_initial_state(iter->first);
    }
}

template <typename State>
void StateMachineBase<State>::set_initial_state(
    StateType::IdType t_stateId) noexcept {
    transition_to(t_stateId);
}

template <typename State>
void StateMachineBase<State>::start() noexcept {
    transition();
}

template <typename State>
void StateMachineBase<State>::exit() noexcept {
    m_nextState = &m_invalidState;
}

template <typename State>
void StateMachineBase<State>::transition() noexcept {
    if (m_nextState != m_currentState) {
        m_currentState->exit();

        m_previousState = m_currentState;
        m_currentState = m_nextState;

        m_currentState->enter();
    }
}

template <typename State>
void StateMachineBase<State>::transition_to(StateType::IdType t_state) noexcept {
    if (m_nextState == m_currentState) {
        if (auto iter{ m_states.find(t_state) }; iter != m_states.end()) {
            m_nextState = &iter->second;
        }
    }
}

template <typename State>
void StateMachineBase<State>::transition_to_previous() noexcept {
    if (m_nextState == m_currentState) {
        m_nextState = m_previousState;
    }
}

using StateMachine = StateMachineBase<State>;

}   // namespace fw
