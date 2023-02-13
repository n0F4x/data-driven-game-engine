#pragma once

namespace app {

template <class AppType>
class ScheduleInterface {
public:
    ///----------------///
    ///  Type aliases  ///
    ///----------------///
    using AppView = typename AppType::AppView;

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
