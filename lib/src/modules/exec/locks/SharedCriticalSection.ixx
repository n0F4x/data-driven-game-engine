module;

#include <optional>
#include <span>
#include <vector>

export module ddge.modules.exec.locks.SharedCriticalSection;

import ddge.modules.exec.locks.ExclusiveCriticalSection;
import ddge.modules.exec.locks.LockOwnerIndex;

namespace ddge::exec {

export class SharedCriticalSection {
public:
    SharedCriticalSection() = default;

    explicit SharedCriticalSection(ExclusiveCriticalSection&& preceeding_section)
        : m_preceeding_section{ std::move(preceeding_section) }
    {}

    [[nodiscard]]
    auto owners() const noexcept -> std::span<const LockOwnerIndex>
    {
        return m_owners;
    }

    auto expand(const LockOwnerIndex new_owner) -> void
    {
        m_owners.push_back(new_owner);
    }

    [[nodiscard]]
    auto preceeding_section() const noexcept
        -> const std::optional<ExclusiveCriticalSection>&
    {
        return m_preceeding_section;
    }

private:
    std::vector<LockOwnerIndex>             m_owners;
    std::optional<ExclusiveCriticalSection> m_preceeding_section;
};

}   // namespace ddge::exec
