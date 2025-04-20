module;

#include <concepts>
#include <type_traits>
#include <utility>

export module core.scheduler.task_wrappers.LoopUntil;

import core.scheduler.as_task;
import core.scheduler.concepts.task_c;
import core.scheduler.concepts.task_predicate_c;
import core.scheduler.task_wrappers.TaskWrapperBase;

namespace core::scheduler {

export template <task_c Task_T, task_predicate_c Predicate_T>
class LoopUntil : public TaskWrapperBase {
public:
    template <typename UTask_T, typename UPredicate_T>
        requires std::constructible_from<Task_T, UTask_T&&>
              && std::constructible_from<Predicate_T, UPredicate_T&&>
    constexpr explicit LoopUntil(UTask_T&& task, UPredicate_T&& predicate);

private:
    Task_T      m_task;
    Predicate_T m_predicate;
};

}   // namespace core::scheduler

template <core::scheduler::task_c Task_T, core::scheduler::task_predicate_c Predicate_T>
template <typename UTask_T, typename UPredicate_T>
    requires std::constructible_from<Task_T, UTask_T&&>
              && std::constructible_from<Predicate_T, UPredicate_T&&>
constexpr core::scheduler::LoopUntil<Task_T, Predicate_T>::LoopUntil(
    UTask_T&&      task,
    UPredicate_T&& predicate
)
    : m_task{ std::forward<UTask_T>(task) },
      m_predicate{ std::forward<UPredicate_T>(predicate) }
{}
