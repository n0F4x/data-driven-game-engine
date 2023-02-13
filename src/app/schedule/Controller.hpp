#pragma once

namespace app {

class Schedule;

class Controller final {
public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit Controller(Schedule& t_schedule) noexcept;

    void quit() noexcept;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    Schedule& m_schedule;
};

}   // namespace app
