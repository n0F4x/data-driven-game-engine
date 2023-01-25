#pragma once

#include "config/config.hpp"
#include "framework/StateMachine.hpp"

namespace engine {

class Controller final {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit Controller(fw::StateMachine& t_stateMachine) noexcept
        : m_stateMachine{ t_stateMachine } {}

    [[nodiscard]] Controller(const Controller&) = delete;
    [[nodiscard]] Controller(Controller&&) noexcept = delete;

    ///--------------------///
    ///  Member functions  ///
    ///--------------------///
    void quit() noexcept;
    void transition_to(config::Id t_nextState) noexcept;
    void transition_to_prev() noexcept;

private:
    ///--------------------///
    ///  Member variables  ///
    ///--------------------///
    fw::StateMachine& m_stateMachine;
};

}   // namespace engine
