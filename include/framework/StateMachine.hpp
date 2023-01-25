#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>

#include "framework/State.hpp"

namespace fw {

template <typename StateId>
class StateMachineBase final {
    using State = StateBase<StateId>;

public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] StateMachineBase() = default;
    [[nodiscard]] StateMachineBase(const StateMachineBase&) = delete;
    [[nodiscard]] StateMachineBase(StateMachineBase&&) noexcept = default;

    ///--------------------///
    ///  Member functions  ///
    ///--------------------///
    [[nodiscard]] auto running() const noexcept -> bool;
    void add_state(State&& t_state, bool t_setAsInitialState = false);
    void set_initial_state(StateId t_stateId) noexcept;
    void start() noexcept;
    void exit() noexcept;
    void transition() noexcept;
    void transition_to(StateId t_stateId) noexcept;
    void transition_to_previous() noexcept;

private:
    ///--------------------///
    ///  Member variables  ///
    ///--------------------///
    std::unique_ptr<std::mutex> m_transitionLock =
        std::make_unique<std::mutex>();
    std::unordered_map<StateId, const State> m_states;
    gsl::not_null<const State*> m_nextState = State::invalid_state();
    gsl::not_null<const State*> m_currentState = State::invalid_state();
    gsl::not_null<const State*> m_prevState = State::invalid_state();
};

template <typename StateId>
auto StateMachineBase<StateId>::running() const noexcept -> bool {
    return !m_currentState->invalid();
}

template <typename StateId>
void StateMachineBase<StateId>::add_state(State&& t_state,
                                          bool t_setAsInitialState) {
    if (auto [iter, success] =
            m_states.try_emplace(t_state.id(), std::move(t_state));
        success && t_setAsInitialState)
    {
        set_initial_state(iter->first);
    }
}

template <typename StateId>
void StateMachineBase<StateId>::set_initial_state(StateId t_stateId) noexcept {
    transition_to(t_stateId);
}

template <typename StateId>
void StateMachineBase<StateId>::start() noexcept {
    transition();
}

template <typename StateId>
void StateMachineBase<StateId>::exit() noexcept {
    m_nextState = State::invalid_state();
}

template <typename StateId>
void StateMachineBase<StateId>::transition() noexcept {
    if (m_nextState != m_currentState) {
        m_currentState->exited();

        m_prevState = m_currentState;
        m_currentState = m_nextState;

        m_currentState->entered();
    }
}

template <typename StateId>
void StateMachineBase<StateId>::transition_to(StateId t_state) noexcept {
    std::lock_guard guard{ *m_transitionLock };
    if (m_nextState == m_currentState) {
        if (auto iter{ m_states.find(t_state) }; iter != m_states.end()) {
            m_nextState = &iter->second;
        }
    }
}

template <typename StateId>
void StateMachineBase<StateId>::transition_to_previous() noexcept {
    std::lock_guard guard{ *m_transitionLock };
    if (m_nextState == m_currentState) {
        m_nextState = m_prevState;
    }
}

using StateMachine = StateMachineBase<unsigned>;

}   // namespace fw
