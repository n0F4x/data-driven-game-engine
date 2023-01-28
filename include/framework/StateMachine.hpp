#pragma once

#include <memory>
#include <unordered_map>

#include "framework/State.hpp"

namespace fw::fsm {

template <class T>
concept StateConcept = requires(T t) {
                           typename T::IdType;
                           typename T::Action;
                           {
                               t.id()
                               } -> std::same_as<typename T::IdType>;
                           {
                               t.enter()
                           };
                           {
                               t.exit()
                           };
                       };

template <StateConcept State>
class BasicStateMachine final {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using StateType = State;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto running() const noexcept -> bool;
    void add_state(StateType&& t_state, bool t_setAsInitialState = false);
    void set_initial_state(StateType::IdType t_stateId) noexcept;
    void start() noexcept;
    void exit() noexcept;
    void transition() noexcept;
    void transition_to(StateType::IdType t_stateId) noexcept;
    void transition_to_previous() noexcept;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    std::unordered_map<typename StateType::IdType, StateType> m_states;
    StateType m_invalidState = StateType{};
    gsl::not_null<State*> m_nextState = &m_invalidState;
    gsl::not_null<State*> m_currentState = &m_invalidState;
    gsl::not_null<State*> m_previousState = &m_invalidState;
};

template <typename State>
auto BasicStateMachine<State>::running() const noexcept -> bool {
    return m_currentState != &m_invalidState;
}

template <typename State>
void BasicStateMachine<State>::add_state(StateType&& t_state,
                                         bool t_setAsInitialState) {
    if (auto [iter, success] =
            m_states.try_emplace(t_state.id(), std::move(t_state));
        success && t_setAsInitialState)
    {
        set_initial_state(iter->first);
    }
}

template <typename State>
void BasicStateMachine<State>::set_initial_state(
    StateType::IdType t_stateId) noexcept {
    transition_to(t_stateId);
}

template <typename State>
void BasicStateMachine<State>::start() noexcept {
    transition();
}

template <typename State>
void BasicStateMachine<State>::exit() noexcept {
    m_nextState = &m_invalidState;
}

template <typename State>
void BasicStateMachine<State>::transition() noexcept {
    if (m_nextState != m_currentState) {
        m_currentState->exit();

        m_previousState = m_currentState;
        m_currentState = m_nextState;

        m_currentState->enter();
    }
}

template <typename State>
void BasicStateMachine<State>::transition_to(
    StateType::IdType t_state) noexcept {
    if (m_nextState == m_currentState) {
        if (auto iter{ m_states.find(t_state) }; iter != m_states.end()) {
            m_nextState = &iter->second;
        }
    }
}

template <typename State>
void BasicStateMachine<State>::transition_to_previous() noexcept {
    if (m_nextState == m_currentState) {
        m_nextState = m_previousState;
    }
}

using StateMachine = BasicStateMachine<State>;

}   // namespace fw::fsm
