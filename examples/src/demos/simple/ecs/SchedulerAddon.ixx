module;

#include <concepts>
#include <type_traits>

export module ecs:SchedulerAddon;

import :Scheduler;
import :SchedulerAddonTag;

import utility.meta.concepts.specialization_of;

namespace ecs {

export template <util::meta::specialization_of_c<::Scheduler> Scheduler_T>
struct SchedulerAddon : ecs::SchedulerAddonTag {
    Scheduler_T scheduler;

    template <typename UScheduler_T>
        requires std::constructible_from<Scheduler_T, UScheduler_T&&>
    constexpr explicit SchedulerAddon(UScheduler_T&& scheduler);
};

export template <typename Scheduler_T>
SchedulerAddon(Scheduler_T&& scheduler)
    -> SchedulerAddon<std::remove_cvref_t<Scheduler_T>>;

}   // namespace ecs

template <util::meta::specialization_of_c<::Scheduler> Scheduler_T>
template <typename UScheduler_T>
    requires std::constructible_from<Scheduler_T, UScheduler_T&&>
constexpr ecs::SchedulerAddon<Scheduler_T>::SchedulerAddon(UScheduler_T&& scheduler)
    : scheduler{ std::forward<UScheduler_T>(scheduler) }
{}
