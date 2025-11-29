module;

#include <variant>
#include <vector>

export module ddge.modules.exec.locks.Lock;

import ddge.modules.exec.locks.CriticalSection;
import ddge.modules.exec.locks.CriticalSectionType;
import ddge.modules.exec.locks.ExclusiveCriticalSection;
import ddge.modules.exec.locks.LockedResourceID;
import ddge.modules.exec.locks.LockOwnerIndex;
import ddge.modules.exec.locks.SharedCriticalSection;
import ddge.modules.exec.locks.TopOfLockStack;

import ddge.utility.Overloaded;

namespace ddge::exec {

export class Lock {
public:
    constexpr explicit Lock(CriticalSectionType type);

    [[nodiscard]]
    constexpr auto overwrite(Lock&& other) const -> Lock;

    [[nodiscard]]
    auto waits_for(
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

}   // namespace ddge::exec

constexpr ddge::exec::Lock::Lock(const CriticalSectionType type) : m_type{ type } {}

constexpr auto ddge::exec::Lock::overwrite(Lock&& other) const -> Lock
{
    switch (m_type) {
        case CriticalSectionType::eExclusive: return *this;
        case CriticalSectionType::eShared:    return std::move(other);
    }
}

[[nodiscard]]
auto exclusive_owner_to_vector(
    const ddge::exec::ExclusiveCriticalSection& critical_section
) -> std::vector<ddge::exec::LockOwnerIndex>
{
    return std::vector<ddge::exec::LockOwnerIndex>{ critical_section.owner() };
}

auto ddge::exec::Lock::waits_for(
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

auto ddge::exec::Lock::transform_top_of_lock_stack(
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

    return std::move(top_of_lock_stack);
}
