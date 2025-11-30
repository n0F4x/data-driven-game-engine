module;

#include <flat_map>
#include <ranges>
#include <vector>

export module ddge.modules.scheduler.locks.LockGroup;

import ddge.modules.scheduler.locks.Lock;
import ddge.modules.scheduler.locks.LockedResourceID;
import ddge.modules.scheduler.locks.LockOwnerIndex;
import ddge.modules.scheduler.locks.TopOfLockStack;

import ddge.utility.meta.concepts.naked;
import ddge.utility.meta.reflection.hash;
import ddge.utility.TypeList;

namespace ddge::scheduler {

export class LockGroup {
public:
    template <util::meta::naked_c T>
    constexpr auto expand(Lock&& lock) -> void;

    constexpr auto expand(const LockGroup& lock_group) -> void;

    [[nodiscard]]
    auto dependencies(const TopOfLockStack& top_of_lock_stack) const
        -> std::vector<LockOwnerIndex>;

    [[nodiscard]]
    auto transform_top_of_lock_stack(
        TopOfLockStack&& top_of_lock_stack,
        LockOwnerIndex   owner
    ) const -> TopOfLockStack;

private:
    std::flat_map<LockedResourceID, Lock> m_locks;

    constexpr auto expand(LockedResourceID resource_id, Lock&& lock) -> void;
};

}   // namespace ddge::scheduler

template <ddge::util::meta::naked_c T>
constexpr auto ddge::scheduler::LockGroup::expand(Lock&& lock) -> void
{
    expand(util::meta::hash<T>(), std::move(lock));
}

constexpr auto ddge::scheduler::LockGroup::expand(const LockGroup& lock_group) -> void
{
    for (const auto& [resource_id, lock] : lock_group.m_locks) {
        expand(resource_id, auto{ lock });
    }
}

auto ddge::scheduler::LockGroup::dependencies(
    const TopOfLockStack& top_of_lock_stack
) const -> std::vector<LockOwnerIndex>
{
    return m_locks
         | std::views::transform([&top_of_lock_stack](auto&& resource_id_and_lock) {
               auto&& [resource_id, lock]{ resource_id_and_lock };
               return lock.dependencies(top_of_lock_stack, resource_id);
           })
         | std::views::join   //
         | std::ranges::to<std::vector<LockOwnerIndex>>();
}

auto ddge::scheduler::LockGroup::transform_top_of_lock_stack(
    TopOfLockStack&&     top_of_lock_stack,
    const LockOwnerIndex owner
) const -> TopOfLockStack
{
    for (auto&& [resource_id, lock] : m_locks) {
        top_of_lock_stack = lock.transform_top_of_lock_stack(
            std::move(top_of_lock_stack), resource_id, owner
        );
    }
    return std::move(top_of_lock_stack);
}

constexpr auto
    ddge::scheduler::LockGroup::expand(const LockedResourceID resource_id, Lock&& lock)
        -> void
{
    if (const auto iter{ m_locks.find(resource_id) }; iter != m_locks.cend()) {
        iter->second = lock.overwrite(std::move(iter->second));
    }
    else {
        m_locks.try_emplace(resource_id, std::move(lock));
    }
}
