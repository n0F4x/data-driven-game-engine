#pragma once

#include "config/id.hpp"

class App;


class Controller final {
public:
    ///------------------------------///
   ///  Constructors / Destructors  ///
  ///------------------------------///
    explicit [[nodiscard]] Controller(App& app) noexcept : app{ app } {}
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
    App& app;
};
