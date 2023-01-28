#pragma once

#include <type_traits>
#include <unordered_map>

#include "framework/State.hpp"

namespace fw::fsm {

template <class State>
concept StateConcept = requires(State t) {
                           {
                               t.id()
                               } -> std::convertible_to<unsigned>;
                           {
                               t.enter()
                           };
                           {
                               t.exit()
                           };
                       } && std::destructible<State>;

///---------------------///
///  BasicStateMachine  ///
///---------------------///
template <StateConcept State>
class BasicStateMachine final {
public:
    ///------------------///
    ///  Nested classes  ///
    ///------------------///
    class Builder;

    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using StateIdType = std::invoke_result_t<decltype(&State::id), State>;
    using StateType = State;
    using StateContainerType = std::unordered_map<StateIdType, StateType>;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit BasicStateMachine(StateContainerType&& t_states = {},
                                             StateIdType t_initialStateId = {});

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto running() const noexcept -> bool;
    void start() noexcept;
    void exit() noexcept;
    void transition() noexcept;
    void set_next_state(StateIdType t_stateId) noexcept;
    void set_next_state_as_previous() noexcept;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    StateContainerType m_states;
    StateType m_invalidState = StateType{};
    gsl::not_null<State*> m_nextState = &m_invalidState;
    gsl::not_null<State*> m_currentState = &m_invalidState;
    gsl::not_null<State*> m_previousState = &m_invalidState;
    bool shouldTransition = true;
};

///------------------------------///
///  BasicStateMachine::Builder  ///
///------------------------------///
template <StateConcept State>
class BasicStateMachine<State>::Builder final {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using ProductType = BasicStateMachine<State>;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] explicit(false) operator ProductType();
    [[nodiscard]] auto build() -> ProductType;

    [[nodiscard]] auto add_state(ProductType::StateType&& t_state,
                                 bool t_setAsInitialState = false) -> Builder&;
    [[nodiscard]] auto set_initial_state(StateIdType t_stateId) noexcept
        -> Builder&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    StateContainerType m_states;
    StateIdType m_initialStateId;
};

////////////////////////////////////////////
///--------------------------------------///
///  BasicStateMachine   IMPLEMENTATION  ///
///--------------------------------------///
////////////////////////////////////////////
template <StateConcept State>
BasicStateMachine<State>::BasicStateMachine(StateContainerType&& t_states,
                                            StateIdType t_initialStateId)
    : m_states{ std::move(t_states) } {
    set_next_state(t_initialStateId);
}

template <StateConcept State>
auto BasicStateMachine<State>::running() const noexcept -> bool {
    return m_currentState != &m_invalidState;
}

template <StateConcept State>
void BasicStateMachine<State>::start() noexcept {
    transition();
}

template <StateConcept State>
void BasicStateMachine<State>::exit() noexcept {
    m_nextState = &m_invalidState;
    shouldTransition = true;
    transition();
}

template <StateConcept State>
void BasicStateMachine<State>::transition() noexcept {
    if (shouldTransition) {
        m_currentState->exit();

        m_previousState = m_currentState;
        m_currentState = m_nextState;

        m_currentState->enter();
    }
    shouldTransition = false;
}

template <StateConcept State>
void BasicStateMachine<State>::set_next_state(StateIdType t_stateId) noexcept {
    if (auto iter{ m_states.find(t_stateId) }; iter != m_states.end()) {
        m_nextState = &iter->second;
        shouldTransition = true;
    }
}

template <StateConcept State>
void BasicStateMachine<State>::set_next_state_as_previous() noexcept {
    m_nextState = m_previousState;
    shouldTransition = true;
}

/////////////////////////////////////////////////////
///-----------------------------------------------///
///  BasicStateMachine::Builder   IMPLEMENTATION  ///
///-----------------------------------------------///
/////////////////////////////////////////////////////
template <StateConcept State>
BasicStateMachine<State>::Builder::operator ProductType() {
    return build();
}

template <StateConcept State>
auto BasicStateMachine<State>::Builder::build() -> ProductType {
    return ProductType{ std::move(m_states), m_initialStateId };
}

template <StateConcept State>
auto BasicStateMachine<State>::Builder::add_state(StateType&& t_state,
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

template <StateConcept State>
auto BasicStateMachine<State>::Builder::set_initial_state(
    StateIdType t_stateId) noexcept -> Builder& {
    m_initialStateId = t_stateId;
    return *this;
}

using StateMachine = BasicStateMachine<State>;

}   // namespace fw::fsm
