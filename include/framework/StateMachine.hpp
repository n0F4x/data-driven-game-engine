#pragma once

#include <memory>
#include <mutex>
#include <unordered_map>

#include "framework/State.hpp"

namespace fw {

template <typename StateId>
class StateMachineBase final {
public:
    ///----------------///
    ///  Member types  ///
    ///----------------///
    using State = StateBase<StateId>;

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
    std::unique_ptr<std::mutex> m_containerLock =
        std::make_unique<std::mutex>();
    std::unique_ptr<std::mutex> m_transitionLock =
        std::make_unique<std::mutex>();
    std::unordered_map<StateId, const State> m_states;
    const State m_invalidState = State::create();
    gsl::not_null<const State*> m_nextState = &m_invalidState;
    gsl::not_null<const State*> m_currentState = &m_invalidState;
    gsl::not_null<const State*> m_previousState = &m_invalidState;
};

template <typename StateId>
auto StateMachineBase<StateId>::running() const noexcept -> bool {
    return m_currentState != &m_invalidState;
}

template <typename StateId>
void StateMachineBase<StateId>::add_state(State&& t_state,
                                          bool t_setAsInitialState) {
    std::lock_guard guard{ *m_containerLock };
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
    std::lock_guard guard{ *m_transitionLock };
    m_nextState = &m_invalidState;
}

template <typename StateId>
void StateMachineBase<StateId>::transition() noexcept {
    std::lock_guard guard{ *m_transitionLock };
    if (m_nextState != m_currentState) {
        m_currentState->exit();

        m_previousState = m_currentState;
        m_currentState = m_nextState;

        m_currentState->enter();
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
        m_nextState = m_previousState;
    }
}

using StateMachine = StateMachineBase<uint32_t>;

}   // namespace fw
