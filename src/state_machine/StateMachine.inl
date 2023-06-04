namespace engine {

///////////////////////////////////////
///---------------------------------///
///  StateMachine   IMPLEMENTATION  ///
///---------------------------------///
///////////////////////////////////////
template <StateConcept StateType>
StateMachine<StateType>::StateMachine(StateContainer&& t_states,
                                      StateId t_initialStateId)
    : m_states{ std::move(t_states) } {
    set_next_state(t_initialStateId);
}

template <StateConcept StateType>
auto StateMachine<StateType>::running() const noexcept -> bool {
    return m_currenStateType != &m_invalidState;
}

template <StateConcept StateType>
void StateMachine<StateType>::start() noexcept {
    transition();
}

template <StateConcept StateType>
void StateMachine<StateType>::exit() noexcept {
    set_next_state(&m_invalidState);
    transition();
}

template <StateConcept StateType>
void StateMachine<StateType>::transition() noexcept {
    if (m_shouldTransition) {
        m_currenStateType->exit();

        m_previousState = m_currenStateType;
        m_currenStateType = m_nexStateType;

        m_currenStateType->enter();
    }
    m_shouldTransition = false;
}

template <StateConcept StateType>
void StateMachine<StateType>::set_next_state(StateId t_stateId) noexcept {
    if (auto iter{ m_states.find(t_stateId) }; iter != m_states.end()) {
        set_next_state(&iter->second);
    }
}

template <StateConcept StateType>
void StateMachine<StateType>::set_next_state_as_previous() noexcept {
    set_next_state(m_previousState);
}

template <StateConcept StateType>
void StateMachine<StateType>::set_next_state(
    gsl::not_null<State*> t_nextState) noexcept {
    m_nexStateType = t_nextState;
    m_shouldTransition = true;
}

////////////////////////////////////////////////
///------------------------------------------///
///  StateMachine::Builder   IMPLEMENTATION  ///
///------------------------------------------///
////////////////////////////////////////////////
template <StateConcept StateType>
StateMachine<StateType>::Builder::operator Product() {
    return build();
}

template <StateConcept StateType>
auto StateMachine<StateType>::Builder::build() -> Product {
    return Product{ std::move(m_states), m_initialStateId };
}

template <StateConcept StateType>
auto StateMachine<StateType>::Builder::add_state(State&& t_state,
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

template <StateConcept StateType>
auto StateMachine<StateType>::Builder::set_initial_state(
    StateId t_stateId) noexcept -> Builder& {
    m_initialStateId = t_stateId;
    return *this;
}

}   // namespace engine
