#pragma once

namespace app {

template <class AppViewType>
class ScheduleInterface {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using AppView = AppViewType;

    ///------------------------------///
    ///  Constructors / Destructors  ///
    ///------------------------------///
    virtual ~ScheduleInterface() = default;

    ///-----------///
    ///  Methods  ///
    ///-----------///
    virtual void execute(AppView) = 0;
};

}   // namespace app
