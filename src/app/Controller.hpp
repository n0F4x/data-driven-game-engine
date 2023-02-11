#pragma once

#include "engine/state_machine/StateMachine.hpp"

namespace app {

class Controller final {
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using StateMachine = engine::StateMachine;

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

}   // namespace app
