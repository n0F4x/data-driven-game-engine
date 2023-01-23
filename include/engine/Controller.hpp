#pragma once

#include "config/id.hpp"

class StateMachine;


class Controller final {
public:
    ///------------------------------///
   ///  Constructors / Destructors  ///
  ///------------------------------///
    [[nodiscard]] explicit Controller(StateMachine& stateMachine) noexcept : stateMachine{ stateMachine } {}
    [[nodiscard]] Controller(const Controller&) = delete;
    [[nodiscard]] Controller(Controller&&) noexcept = delete;

    ///--------------------///
   ///  Member functions  ///
  ///--------------------///
    void quit() noexcept;
    void transition_to(Id to) noexcept;
    void transition_to_prev() noexcept;

private:
    ///--------------------///
   ///  Member variables  ///
  ///--------------------///
    StateMachine& stateMachine;
};
