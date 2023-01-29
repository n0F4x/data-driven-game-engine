#pragma once

#include <unordered_map>

#include <gsl/pointers>

#include "framework/State.hpp"

namespace fw::fsm {

template <class TState>
concept StateConcept = requires(TState t) {
                           {
                               t.id()
                               } -> std::convertible_to<unsigned>;
                           {
                               t.enter()
                           };
                           {
                               t.exit()
                           };
                       } && std::destructible<TState>;

///---------------------///
///  BasicStateMachine  ///
///---------------------///
template <StateConcept TState>
class BasicStateMachine final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using StateId = std::invoke_result_t<decltype(&State::id), State>;
    using State = TState;
    using StateContainer = std::unordered_map<StateId, State>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit BasicStateMachine(StateContainer&& t_states = {},
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
    ///-------------///
    ///  Variables  ///
    ///-------------///
    StateContainer m_states;
    State m_invalidState = State{};
    gsl::not_null<State*> m_nextState = &m_invalidState;
    gsl::not_null<State*> m_currentState = &m_invalidState;
    gsl::not_null<State*> m_previousState = &m_invalidState;
    bool shouldTransition = true;
};

///------------------------------///
///  BasicStateMachine::Builder  ///
///------------------------------///
template <StateConcept TState>
class BasicStateMachine<TState>::Builder final {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Product = BasicStateMachine<TState>;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] explicit(false) operator Product();
    [[nodiscard]] auto build() -> Product;

    [[nodiscard]] auto add_state(State&& t_state,
                                 bool t_setAsInitialState = false) -> Builder&;
    [[nodiscard]] auto set_initial_state(StateId t_stateId) noexcept
        -> Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    StateContainer m_states;
    StateId m_initialStateId;
};

using StateMachine = BasicStateMachine<State>;

}   // namespace fw::fsm

#include "framework/StateMachine.inl"
