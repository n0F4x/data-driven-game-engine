namespace fw::fsm {

////////////////////////////////////////////
///--------------------------------------///
///  BasicStateMachine   IMPLEMENTATION  ///
///--------------------------------------///
////////////////////////////////////////////
template <StateConcept StateType>
BasicStateMachine<StateType>::BasicStateMachine(StateContainer&& t_states,
                                                StateId t_initialStateId)
    : m_states{ std::move(t_states) } {
    set_next_state(t_initialStateId);
}

template <StateConcept StateType>
auto BasicStateMachine<StateType>::running() const noexcept -> bool {
    return m_currenStateType != &m_invalidState;
}

template <StateConcept StateType>
void BasicStateMachine<StateType>::start() noexcept {
    transition();
}

template <StateConcept StateType>
void BasicStateMachine<StateType>::exit() noexcept {
    m_nexStateType = &m_invalidState;
    shouldTransition = true;
    transition();
}

template <StateConcept StateType>
void BasicStateMachine<StateType>::transition() noexcept {
    if (shouldTransition) {
        m_currenStateType->exit();

        m_previousState = m_currenStateType;
        m_currenStateType = m_nexStateType;

        m_currenStateType->enter();
    }
    shouldTransition = false;
}

template <StateConcept StateType>
void BasicStateMachine<StateType>::set_next_state(StateId t_stateId) noexcept {
    if (auto iter{ m_states.find(t_stateId) }; iter != m_states.end()) {
        m_nexStateType = &iter->second;
        shouldTransition = true;
    }
}

template <StateConcept StateType>
void BasicStateMachine<StateType>::set_next_state_as_previous() noexcept {
    m_nexStateType = m_previousState;
    shouldTransition = true;
}

/////////////////////////////////////////////////////
///-----------------------------------------------///
///  BasicStateMachine::Builder   IMPLEMENTATION  ///
///-----------------------------------------------///
/////////////////////////////////////////////////////
template <StateConcept StateType>
BasicStateMachine<StateType>::Builder::operator Product() {
    return build();
}

template <StateConcept StateType>
auto BasicStateMachine<StateType>::Builder::build() -> Product {
    return Product{ std::move(m_states), m_initialStateId };
}

template <StateConcept StateType>
auto BasicStateMachine<StateType>::Builder::add_state(State&& t_state,
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
auto BasicStateMachine<StateType>::Builder::set_initial_state(
    StateId t_stateId) noexcept -> Builder& {
    m_initialStateId = t_stateId;
    return *this;
}

}   // namespace fw::fsm
