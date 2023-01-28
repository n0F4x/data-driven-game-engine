#pragma once

#include <memory>
#include <unordered_map>

#include "framework/State.hpp"

namespace fw::fsm {

template <class State, typename Id>
concept StateConcept = requires(State t) {
                           {
                               t.id()
                               } -> std::convertible_to<Id>;
                           {
                               t.enter()
                           };
                           {
                               t.exit()
                           };
                       } && std::destructible<State>;

template <typename StateId, StateConcept<StateId> State>
class BasicStateMachine final {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using StateIdType = StateId;
    using StateType = State;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto running() const noexcept -> bool;
    void add_state(StateType&& t_state, bool t_setAsInitialState = false);
    void set_initial_state(StateIdType t_stateId) noexcept;
    void start() noexcept;
    void exit() noexcept;
    void transition() noexcept;
    void transition_to(StateIdType t_stateId) noexcept;
    void transition_to_previous() noexcept;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    std::unordered_map<StateIdType, StateType> m_states;
    StateType m_invalidState = StateType{};
    gsl::not_null<State*> m_nextState = &m_invalidState;
    gsl::not_null<State*> m_currentState = &m_invalidState;
    gsl::not_null<State*> m_previousState = &m_invalidState;
};

template <typename StateId, StateConcept<StateId> State>
auto BasicStateMachine<StateId, State>::running() const noexcept -> bool {
    return m_currentState != &m_invalidState;
}

template <typename StateId, StateConcept<StateId> State>
void BasicStateMachine<StateId, State>::add_state(StateType&& t_state,
                                                  bool t_setAsInitialState) {
    if (auto [iter, success] =
            m_states.try_emplace(t_state.id(), std::move(t_state));
        success && t_setAsInitialState)
    {
        set_initial_state(iter->first);
    }
}

template <typename StateId, StateConcept<StateId> State>
void BasicStateMachine<StateId, State>::set_initial_state(
    StateIdType t_stateId) noexcept {
    transition_to(t_stateId);
}

template <typename StateId, StateConcept<StateId> State>
void BasicStateMachine<StateId, State>::start() noexcept {
    transition();
}

template <typename StateId, StateConcept<StateId> State>
void BasicStateMachine<StateId, State>::exit() noexcept {
    m_nextState = &m_invalidState;
}

template <typename StateId, StateConcept<StateId> State>
void BasicStateMachine<StateId, State>::transition() noexcept {
    if (m_nextState != m_currentState) {
        m_currentState->exit();

        m_previousState = m_currentState;
        m_currentState = m_nextState;

        m_currentState->enter();
    }
}

template <typename StateId, StateConcept<StateId> State>
void BasicStateMachine<StateId, State>::transition_to(
    StateIdType t_stateId) noexcept {
    if (m_nextState == m_currentState) {
        if (auto iter{ m_states.find(t_stateId) }; iter != m_states.end()) {
            m_nextState = &iter->second;
        }
    }
}

template <typename StateId, StateConcept<StateId> State>
void BasicStateMachine<StateId, State>::transition_to_previous() noexcept {
    if (m_nextState == m_currentState) {
        m_nextState = m_previousState;
    }
}

using StateMachine = BasicStateMachine<State::IdType, State>;

}   // namespace fw::fsm
