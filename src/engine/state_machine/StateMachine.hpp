#pragma once

#include <unordered_map>

#include <gsl/pointers>

#include "engine/core/state.hpp"

namespace engine {

///---------------------///
///  BasicStateMachine  ///
///---------------------///
template <StateConcept StateType>
class StateMachine final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using State = StateType;
    using StateId = std::invoke_result_t<decltype(&State::id), State>;
    using StateContainer = std::unordered_map<StateId, State>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit StateMachine(StateContainer&& t_states = {},
                                             StateId t_initialStateId = {});

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto running() const noexcept -> bool;
    void start() noexcept;
    void exit() noexcept;
    void transition() noexcept;
    void set_next_state(StateId t_stateId) noexcept;
    void set_next_state_as_previous() noexcept;

private:
    void set_next_state(gsl::not_null<State*> t_nextState) noexcept;
    
    ///-------------///
    ///  Variables  ///
    ///-------------///
    StateContainer m_states;
    State m_invalidState = State{};
    gsl::not_null<State*> m_nexStateType = &m_invalidState;
    gsl::not_null<State*> m_currenStateType = &m_invalidState;
    gsl::not_null<State*> m_previousState = &m_invalidState;
    bool m_shouldTransition = true;
};

///------------------------------///
///  BasicStateMachine::Builder  ///
///------------------------------///
template <StateConcept StateType>
class StateMachine<StateType>::Builder final {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Product = StateMachine<StateType>;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] explicit(false) operator Product();
    [[nodiscard]] auto build() -> Product;

    [[nodiscard]] auto
    add_state(State&& t_state, bool t_setAsInitialState = false) -> Builder&;
    [[nodiscard]] auto set_initial_state(StateId t_stateId) noexcept
        -> Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    StateContainer m_states;
    StateId m_initialStateId;
};

}   // namespace engine

#include "StateMachine.inl"
