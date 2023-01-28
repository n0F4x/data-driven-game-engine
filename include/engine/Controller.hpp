#pragma once

#include "config/config.hpp"
#include "framework/StateMachine.hpp"

namespace engine {

class Controller final {
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using StateMachine = fw::fsm::StateMachine;

public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit Controller(StateMachine& t_stateMachine) noexcept
        : m_stateMachine{ t_stateMachine } {}

    [[nodiscard]] Controller(const Controller&) = delete;
    [[nodiscard]] Controller(Controller&&) noexcept = delete;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    void quit() noexcept;
    void transition_to(StateMachine::StateIdType t_nextStateId) noexcept;
    void transition_to_prev() noexcept;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    StateMachine& m_stateMachine;
};

}   // namespace engine
