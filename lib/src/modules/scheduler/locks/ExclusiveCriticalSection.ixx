export module ddge.modules.scheduler.locks.ExclusiveCriticalSection;

import ddge.modules.scheduler.locks.LockOwnerIndex;

namespace ddge::scheduler {

export class ExclusiveCriticalSection {
public:
    explicit ExclusiveCriticalSection(const LockOwnerIndex owner) : m_owner{ owner } {}

    [[nodiscard]]
    auto owner() const noexcept -> LockOwnerIndex
    {
        return m_owner;
    }

private:
    LockOwnerIndex m_owner;
};

}   // namespace ddge::scheduler
