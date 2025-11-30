module;

#include <variant>
#include <vector>

export module ddge.modules.scheduler.locks.Lock;

import ddge.modules.scheduler.locks.CriticalSection;
import ddge.modules.scheduler.locks.CriticalSectionType;
import ddge.modules.scheduler.locks.ExclusiveCriticalSection;
import ddge.modules.scheduler.locks.LockedResourceID;
import ddge.modules.scheduler.locks.LockOwnerIndex;
import ddge.modules.scheduler.locks.SharedCriticalSection;
import ddge.modules.scheduler.locks.TopOfLockStack;

import ddge.utility.Overloaded;

namespace ddge::scheduler {

export class Lock {
public:
    constexpr explicit Lock(CriticalSectionType type);

    [[nodiscard]]
    constexpr auto overwrite(Lock&& other) const -> Lock;

    [[nodiscard]]
    auto dependencies(
        const TopOfLockStack& top_of_lock_stack,
        LockedResourceID      resource_id
    ) const -> std::vector<LockOwnerIndex>;
    [[nodiscard]]
    auto transform_top_of_lock_stack(
        TopOfLockStack&& top_of_lock_stack,
        LockedResourceID resource_id,
        LockOwnerIndex   owner
    ) const -> TopOfLockStack;

private:
    CriticalSectionType m_type;
};

}   // namespace ddge::scheduler

constexpr ddge::scheduler::Lock::Lock(const CriticalSectionType type) : m_type{ type } {}

constexpr auto ddge::scheduler::Lock::overwrite(Lock&& other) const -> Lock
{
    switch (m_type) {
        case CriticalSectionType::eExclusive: return *this;
        case CriticalSectionType::eShared:    return std::move(other);
    }
}

[[nodiscard]]
auto exclusive_owner_to_vector(
    const ddge::scheduler::ExclusiveCriticalSection& critical_section
) -> std::vector<ddge::scheduler::LockOwnerIndex>
{
    return std::vector<ddge::scheduler::LockOwnerIndex>{ critical_section.owner() };
}

auto ddge::scheduler::Lock::dependencies(
    const TopOfLockStack&  top_of_lock_stack,
    const LockedResourceID resource_id
) const -> std::vector<LockOwnerIndex>
{
    if (const auto iter = top_of_lock_stack.find(resource_id);
        iter != top_of_lock_stack.cend())
    {
        const CriticalSection& preceeding_section{ iter->second };
        switch (m_type) {
            case CriticalSectionType::eExclusive:
                return preceeding_section.visit(
                    util::Overloaded{
                        ::exclusive_owner_to_vector,
                        [](const SharedCriticalSection& critical_section) {
                            return std::vector<LockOwnerIndex>{
                                std::from_range,
                                critical_section.owners()   //
                            };
                        },
                    }
                );
            case CriticalSectionType::eShared:
                return preceeding_section.visit(
                    util::Overloaded{
                        ::exclusive_owner_to_vector,
                        [](const SharedCriticalSection& critical_section) {
                            return critical_section.preceeding_section()
                                .transform(::exclusive_owner_to_vector)
                                .value_or(std::vector<LockOwnerIndex>{});
                        },
                    }
                );
        }
        std::unreachable();
    }

    return std::vector<LockOwnerIndex>{};
}

auto ddge::scheduler::Lock::transform_top_of_lock_stack(
    TopOfLockStack&&       top_of_lock_stack,
    const LockedResourceID resource_id,
    const LockOwnerIndex   owner
) const -> TopOfLockStack
{
    if (const auto iter = top_of_lock_stack.find(resource_id);
        iter != top_of_lock_stack.cend())
    {
        CriticalSection& top_critical_section{ iter->second };

        switch (m_type) {
            case CriticalSectionType::eExclusive:
                top_critical_section = ExclusiveCriticalSection{ owner };
                break;
            case CriticalSectionType::eShared:
                top_critical_section = std::visit(
                    util::Overloaded{
                        [owner](ExclusiveCriticalSection&& exclusive_critical_section) {
                            SharedCriticalSection new_shared_critical_section{
                                std::move(exclusive_critical_section)
                            };
                            new_shared_critical_section.expand(owner);
                            return new_shared_critical_section;
                        },
                        [owner](SharedCriticalSection&& shared_critical_section) {
                            shared_critical_section.expand(owner);
                            return std::move(shared_critical_section);
                        },
                    },
                    std::move(top_critical_section)
                );
                break;
        }
    }
    else {
        switch (m_type) {
            case CriticalSectionType::eExclusive:
                top_of_lock_stack.try_emplace(
                    resource_id, ExclusiveCriticalSection{ owner }
                );
                break;
            case CriticalSectionType::eShared:
                SharedCriticalSection new_shared_critical_section;
                new_shared_critical_section.expand(owner);
                top_of_lock_stack.try_emplace(
                    resource_id, std::move(new_shared_critical_section)
                );
                break;
        }
    }

    return std::move(top_of_lock_stack);
}
