#pragma once

#include <concepts>

namespace app {

template <template <class> class ScheduleType, class AppView>
concept ScheduleConcept =
    requires(ScheduleType<AppView> schedule, AppView appView) {
        requires std::constructible_from<ScheduleType<AppView>>;
        requires std::movable<ScheduleType<AppView>>;
        schedule.execute(appView);
    };

template <template <class> class DerivedType, class AppView>
class ScheduleInterface {
public:
    ScheduleInterface()
        requires(ScheduleConcept<DerivedType, AppView>)
    = default;
};

}   // namespace app
