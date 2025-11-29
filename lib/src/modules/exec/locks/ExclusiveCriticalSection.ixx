export module ddge.modules.exec.locks.ExclusiveCriticalSection;

import ddge.modules.exec.locks.LockOwnerIndex;

namespace ddge::exec {

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

}   // namespace ddge::exec
