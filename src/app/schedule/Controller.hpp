#pragma once

namespace app {

template <class ScheduleType>
class Controller final {
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using Schedule = ScheduleType;

public:
    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    [[nodiscard]] explicit Controller(Schedule& t_schedule) noexcept;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    [[nodiscard]] auto running() const noexcept -> bool;
    void quit() noexcept;

private:
    ///-------------///
    ///  Variables  ///
    ///-------------///
    Schedule& m_schedule;
};

}   // namespace app

#include "Controller.inl"
