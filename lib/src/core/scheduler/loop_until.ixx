module;

#include <type_traits>

export module core.scheduler.loop_until;

import core.scheduler.as_task;
import core.scheduler.concepts.converts_to_task_c;
import core.scheduler.concepts.converts_to_task_predicate_c;
import core.scheduler.task_wrappers.LoopUntil;

namespace core::scheduler {

export template <converts_to_task_c Task_T, converts_to_task_predicate_c Predicate_T>
[[nodiscard]]
constexpr auto loop_until(Task_T&& task, Predicate_T&& predicate)
    -> LoopUntil<as_task_t<Task_T>, as_task_t<Predicate_T>>;

}   // namespace core::scheduler

template <
    core::scheduler::converts_to_task_c           Task_T,
    core::scheduler::converts_to_task_predicate_c Predicate_T>
constexpr auto core::scheduler::loop_until(Task_T&& task, Predicate_T&& predicate)
    -> LoopUntil<as_task_t<Task_T>, as_task_t<Predicate_T>>
{
    return LoopUntil<as_task_t<Task_T>, as_task_t<Predicate_T>>{
        std::forward<Task_T>(task), std::forward<Predicate_T>(predicate)
    };
}
