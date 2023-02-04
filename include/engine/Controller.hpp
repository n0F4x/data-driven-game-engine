#pragma once

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
    [[nodiscard]] explicit Controller(StateMachine& t_stateMachine) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto running() const noexcept -> bool;
    void quit() noexcept;
    [[nodiscard]] auto stateMachine() noexcept -> StateMachine&;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    bool m_running = true;
    StateMachine& m_stateMachine;
};

}   // namespace engine
