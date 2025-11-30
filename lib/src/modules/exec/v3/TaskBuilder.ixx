module;

#include <atomic>
#include <concepts>
#include <cstdint>
#include <memory>
#include <optional>
#include <ranges>
#include <utility>
#include <vector>

export module ddge.modules.exec.v3.TaskBuilder;

import ddge.modules.exec.locks.LockGroup;
import ddge.modules.exec.locks.DependencyStack;
import ddge.modules.exec.Nexus;
import ddge.utility.containers.AnyMoveOnlyFunction;
import ddge.utility.meta.concepts.decayed;

namespace ddge::exec::v3 {

enum struct ExecutionMode {
    eGeneral,
    eForceOnMain,
};

using TaskIndex = std::uint64_t;

template <util::meta::decayed_c Result_T>
using TaskContinuation = util::AnyMoveOnlyFunction<void(Result_T&&)>;

class TaskHub {
public:
    auto schedule(TaskIndex task_index) -> void;
};

template <util::meta::decayed_c Result_T>
class EmbeddedTask {
public:
    using ExecuteFunc = util::AnyMoveOnlyFunction<Result_T()>;

    explicit EmbeddedTask(ExecuteFunc&& execute, LockGroup&& lock_group)
        : m_lock_group{ std::move(lock_group) },
          m_execute{ std::move(execute) }
    {}

    auto execute() -> void
    {
        if constexpr (std::is_void_v<Result_T>) {
            m_execute();
            if (m_continuation.has_value()) {
                m_continuation.operator->()();
            }
        }
        else {
            if (m_continuation.has_value()) {
                m_continuation.operator->()(m_execute());
            }
            else {
                m_execute();
            }
        }
    }

    [[nodiscard]]
    auto locks() const noexcept -> const LockGroup&
    {
        return m_lock_group;
    }

    auto set_continuation(TaskContinuation<Result_T>&& continuation) -> void
    {
        m_continuation = std::move(continuation);
    }

private:
    LockGroup                                 m_lock_group;
    ExecuteFunc                               m_execute;
    std::optional<TaskContinuation<Result_T>> m_continuation;
};

template <typename Result_T>
class IndirectTask {
public:
    using ScheduleFunc = util::AnyMoveOnlyFunction<void(TaskHub&)>;
    using SetContinuationFunc =
        util::AnyMoveOnlyFunction<void(TaskContinuation<Result_T>)>;

    explicit IndirectTask(
        ScheduleFunc&&        schedule,
        LockGroup&&           lock_group,
        SetContinuationFunc&& set_continuation_func
    )
        : m_schedule{ std::move(schedule) },
          m_lock_group{ std::move(lock_group) },
          m_set_continuation_func{ std::move(set_continuation_func) }
    {}

    auto schedule(TaskHub& task_hub) -> void
    {
        m_schedule(task_hub);
    }

    [[nodiscard]]
    auto lock_group() const noexcept -> const LockGroup&
    {
        return m_lock_group;
    }

    auto set_continuation(TaskContinuation<Result_T>&& continuation) -> void
    {
        m_set_continuation_func(std::move(continuation));
    }

private:
    ScheduleFunc        m_schedule;
    LockGroup           m_lock_group;
    SetContinuationFunc m_set_continuation_func;
};

class TaskHubBuilder {
public:
    template <typename Result_T>
    [[nodiscard]]
    auto emplace_embedded(EmbeddedTask<Result_T>&& task, ExecutionMode execution_mode)
        -> TaskIndex;

    template <typename Result_T>
    [[nodiscard]]
    auto emplace_indirect(IndirectTask<Result_T>&& task) -> TaskIndex;

    [[nodiscard]]
    auto locks_of(TaskIndex task_index) const -> const LockGroup&;

    template <typename Result_T>
    auto set_continuation(TaskIndex task_index, TaskContinuation<Result_T>&& continuation)
        -> void;
};

template <typename Result_T>
class TaskBuilder {
public:
    using BuildFunc =
        util::AnyMoveOnlyFunction<auto(Nexus&, TaskHubBuilder&) &&->TaskIndex>;

    explicit TaskBuilder(BuildFunc&& build) : m_build{ std::move(build) } {}

    [[nodiscard]]
    auto build(Nexus& nexus, TaskHubBuilder& task_hub_builder) && -> TaskIndex
    {
        return std::move(m_build)(nexus, task_hub_builder);
    }

private:
    BuildFunc m_build;
};

class Gatherer {
public:
    struct Precondition {
        Precondition(uint32_t capacity, const TaskContinuation<bool>& callback);
    };

    explicit Gatherer(uint32_t capacity);

    auto receive(bool value) -> void;

    auto set_callback(TaskContinuation<bool>&& callback) -> void
    {
        m_callback = std::move(callback);
    }

private:
    [[no_unique_address]]
    Precondition m_precondition;

    uint32_t                              m_capacity;
    std::atomic_uint32_t                  m_flag;
    std::atomic_bool                      m_accumulated_result;
    std::optional<TaskContinuation<bool>> m_callback;
};

template <std::same_as<bool>... Bools_T>
[[nodiscard]]
auto all_of(TaskBuilder<Bools_T>&&... task_builders) -> TaskBuilder<bool>
{
    return TaskBuilder<bool>{
        TaskBuilder<bool>::BuildFunc{
            [... x_task_builders = std::move(task_builders)](
                Nexus& nexus, TaskHubBuilder& task_hub_builder
            ) mutable -> TaskIndex {
                std::shared_ptr<Gatherer> gatherer{
                    std::make_shared<Gatherer>(sizeof...(x_task_builders))
                };

                const auto gather{ [gatherer](const bool result) mutable -> void {
                    gatherer->receive(result);
                } };

                std::vector<TaskIndex> task_indices;
                task_indices.reserve(sizeof...(x_task_builders));
                (task_indices.push_back(
                     std::move(x_task_builders).build(nexus, task_hub_builder, gather)
                 ),
                 ...);

                DependencyStack dependency_stack;
                for (const TaskIndex task_index : task_indices) {
                    dependency_stack.emplace(   //
                        task_hub_builder.locks_of(task_index)
                    );
                }

                LockGroup merged_lock_group;
                for (const TaskIndex task_index : task_indices) {
                    merged_lock_group.expand(   //
                        task_hub_builder.locks_of(task_index)
                    );
                }

                return TaskIndex{ task_hub_builder.emplace_indirect(
                    IndirectTask<bool>{
                        [x_task_indices =
                             std::move(task_indices)](TaskHub& task_hub) mutable -> void {
                            for (const TaskIndex task_index : x_task_indices) {
                                task_hub.schedule(task_index);
                            }
                        },
                        std::move(merged_lock_group),
                        [x_gatherer = std::move(gatherer)](
                            TaskContinuation<bool>&& continuation
                        ) mutable {
                            x_gatherer->set_callback(std::move(continuation));
                        }   //
                    }
                ) };
            }   //
        }   //
    };
}

}   // namespace ddge::exec::v3
