#pragma once

#include <type_traits>
#include <unordered_map>

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

    [[nodiscard]] auto add_state(Product::State&& t_state,
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

////////////////////////////////////////////
///--------------------------------------///
///  BasicStateMachine   IMPLEMENTATION  ///
///--------------------------------------///
////////////////////////////////////////////
template <StateConcept TState>
BasicStateMachine<TState>::BasicStateMachine(StateContainer&& t_states,
                                            StateId t_initialStateId)
    : m_states{ std::move(t_states) } {
    set_next_state(t_initialStateId);
}

template <StateConcept TState>
auto BasicStateMachine<TState>::running() const noexcept -> bool {
    return m_currentState != &m_invalidState;
}

template <StateConcept TState>
void BasicStateMachine<TState>::start() noexcept {
    transition();
}

template <StateConcept TState>
void BasicStateMachine<TState>::exit() noexcept {
    m_nextState = &m_invalidState;
    shouldTransition = true;
    transition();
}

template <StateConcept TState>
void BasicStateMachine<TState>::transition() noexcept {
    if (shouldTransition) {
        m_currentState->exit();

        m_previousState = m_currentState;
        m_currentState = m_nextState;

        m_currentState->enter();
    }
    shouldTransition = false;
}

template <StateConcept TState>
void BasicStateMachine<TState>::set_next_state(StateId t_stateId) noexcept {
    if (auto iter{ m_states.find(t_stateId) }; iter != m_states.end()) {
        m_nextState = &iter->second;
        shouldTransition = true;
    }
}

template <StateConcept TState>
void BasicStateMachine<TState>::set_next_state_as_previous() noexcept {
    m_nextState = m_previousState;
    shouldTransition = true;
}

/////////////////////////////////////////////////////
///-----------------------------------------------///
///  BasicStateMachine::Builder   IMPLEMENTATION  ///
///-----------------------------------------------///
/////////////////////////////////////////////////////
template <StateConcept TState>
BasicStateMachine<TState>::Builder::operator Product() {
    return build();
}

template <StateConcept TState>
auto BasicStateMachine<TState>::Builder::build() -> Product {
    return Product{ std::move(m_states), m_initialStateId };
}

template <StateConcept TState>
auto BasicStateMachine<TState>::Builder::add_state(State&& t_state,
                                                  bool t_setAsInitialState)
    -> Builder& {
    if (auto [iter, success] =
            m_states.try_emplace(t_state.id(), std::move(t_state));
        success && t_setAsInitialState)
    {
        return set_initial_state(iter->first);
    }
    return *this;
}

template <StateConcept TState>
auto BasicStateMachine<TState>::Builder::set_initial_state(
    StateId t_stateId) noexcept -> Builder& {
    m_initialStateId = t_stateId;
    return *this;
}

using StateMachine = BasicStateMachine<State>;

}   // namespace fw::fsm
