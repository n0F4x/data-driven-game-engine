export module ecs:SchedulerAddon;

import :Scheduler;

import utility.meta.concepts.specialization_of;

namespace ecs {

export template <util::meta::specialization_of_c<::Scheduler> Scheduler_T>
struct SchedulerAddon {
    Scheduler_T scheduler;
};

}   // namespace ecs
