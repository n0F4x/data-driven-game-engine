namespace fw::fsm {

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

}   // namespace fw::fsm
