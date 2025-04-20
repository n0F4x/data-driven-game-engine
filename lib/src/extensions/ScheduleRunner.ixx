module;

#include <type_traits>

export module extensions.Scheduler;

import utility.meta.concepts.specialization_of;

import core.app.builder_c;

import core.scheduler.task_wrappers.Schedule;

namespace extensions {

export class ScheduleRunner {
public:
    template <core::app::builder_c Self_T, typename Schedule_T>
        requires util::meta::
            specialization_of_c<std::remove_cvref_t<Schedule_T>, core::scheduler::Schedule>
        constexpr auto run(this Self_T&& self, Schedule_T&& schedule) -> void;
};

}   // namespace extensions

template <core::app::builder_c Self_T, typename Schedule_T>
    requires util::meta::
        specialization_of_c<std::remove_cvref_t<Schedule_T>, core::scheduler::Schedule>
    constexpr auto extensions::ScheduleRunner::run(this Self_T&&, Schedule_T&&) -> void
{}
